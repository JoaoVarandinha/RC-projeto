#include "api.h"
#include "parser.h"
#include "client_main.h"
#include "commands.h"
#include "messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>

struct addrinfo *res;
struct sockaddr_in addr;

int connectUDP(connection_info *cInfo){

    int fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1){
        printf(SOCKET_ERROR_MESSAGE);
        return UDP_CONNECT_ERROR;
    }

    cInfo->sockfd = fd;

    struct timeval timeout;
    timeout.tv_sec = RECV_TIMEOUT;
    timeout.tv_usec = 0;
    //Without the timeout a lost datagram would block the client forever
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1){
        printf(SET_TIMEOUT_ERROR_MESSAGE);
        close(fd);
        return UDP_CONNECT_ERROR;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    int errcode = getaddrinfo(cInfo->dsip, cInfo->dsport, &hints, &res);
    if (errcode != 0){
        printf(UDP_CONNECT_ERROR_MESSAGE);
        close(fd);
        return UDP_CONNECT_ERROR;
    }
    return 0;
}

void disconnectUDP(int fd){
    freeaddrinfo(res);
    close(fd);
}

//Opens a TCP connection to the DS, which listens for TCP on the same port
//number it uses for UDP. Returns the connected socket, or TCP_CONNECT_ERROR, so
//that a failed versions command does not bring down the whole application.
int connectTCP(connection_info cInfo){
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    //Kept local so the DS address connectUDP left in res is not overwritten
    struct addrinfo *tcpRes;
    if (getaddrinfo(cInfo.dsip, cInfo.dsport, &hints, &tcpRes) != 0){
        printf(TCP_CONNECT_ERROR_MESSAGE);
        return TCP_CONNECT_ERROR;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1){
        printf(SOCKET_ERROR_MESSAGE);
        freeaddrinfo(tcpRes);
        return TCP_CONNECT_ERROR;
    }

    struct timeval timeout;
    timeout.tv_sec = RECV_TIMEOUT;
    timeout.tv_usec = 0;
    //Without the timeout an unresponsive DS would block the client forever
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1){
        printf(SET_TIMEOUT_ERROR_MESSAGE);
        freeaddrinfo(tcpRes);
        close(fd);
        return TCP_CONNECT_ERROR;
    }

    if (connect(fd, tcpRes->ai_addr, tcpRes->ai_addrlen) == -1){
        printf(TCP_CONNECT_ERROR_MESSAGE);
        freeaddrinfo(tcpRes);
        close(fd);
        return TCP_CONNECT_ERROR;
    }

    //TCP keeps the peer address in the socket, so the list is no longer needed
    freeaddrinfo(tcpRes);
    return fd;
}

void disconnectTCP(int fd){
    close(fd);
}

int sendUDP(int fd, const void* buf, size_t n, int flags, struct addrinfo* res){
    ssize_t s = sendto(fd, buf, n, flags, res->ai_addr, res->ai_addrlen);
    if (s == -1){
        printf(SEND_UDP_ERROR_MESSAGE);
        return SEND_UDP_ERROR;
    }
    return s;
}

int recvUDP(int fd, void* buf, size_t n, int flags, struct sockaddr_in* addr){
    socklen_t addrlen = sizeof(struct sockaddr_in);
    ssize_t s = recvfrom(fd, buf, n, flags, (struct sockaddr*) addr, &addrlen);
    if (s == -1){
        if (errno == EAGAIN || errno == EWOULDBLOCK){
            printf(DS_TIMEOUT_MESSAGE);
            return RECV_TIMEOUT_ERROR;
        }
        printf(RECV_UDP_ERROR_MESSAGE);
        return RECV_UDP_ERROR;
    }
    return s;
}

int client_login(connection_info cInfo, user_info uInfo){
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s %s\n", REQ_LOGIN, uInfo.UID, uInfo.password, cInfo.peerport);

    if (sendUDP(cInfo.sockfd, buf, strlen(buf), 0, res) == SEND_UDP_ERROR) return REQUEST_NOT_SENT;

    memset(buf, 0, sizeof(buf));

    int received = recvUDP(cInfo.sockfd, buf, sizeof(buf), 0, &addr);
    if (received == RECV_TIMEOUT_ERROR) return DS_TIMEOUT;
    if (received == RECV_UDP_ERROR) return REPLY_NOT_RECEIVED;

    char* token = strtok(buf, " \n");
    if (token == NULL || strcmp(token, ANS_LOGIN)) return LOGIN_ERROR;

    char* status = strtok(NULL, " \n");
    if (status == NULL) return LOGIN_ERROR;

    if (strcmp(status, "OK") == 0) return LOGIN_SUCCESS;
    if (strcmp(status, "NOK") == 0) return LOGIN_WRONG_PASSWORD;
    if (strcmp(status, "REG") == 0) return LOGIN_NEW_USER;
    return LOGIN_ERROR;
}

int client_logout(connection_info cInfo, user_info uInfo){
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s\n", REQ_LOGOUT, uInfo.UID, uInfo.password);

    if (sendUDP(cInfo.sockfd, buf, strlen(buf), 0, res) == SEND_UDP_ERROR) return REQUEST_NOT_SENT;

    memset(buf, 0, sizeof(buf));

    int received = recvUDP(cInfo.sockfd, buf, sizeof(buf), 0, &addr);
    if (received == RECV_TIMEOUT_ERROR) return DS_TIMEOUT;
    if (received == RECV_UDP_ERROR) return REPLY_NOT_RECEIVED;

    char* token = strtok(buf, " \n");
    if (token == NULL || strcmp(token, ANS_LOGOUT)) return LOGOUT_ERROR;

    char* status = strtok(NULL, " \n");
    if (status == NULL) return LOGOUT_ERROR;

    if (strcmp(status, "OK") == 0) return LOGOUT_SUCCESS;
    if (strcmp(status, "NLG") == 0) return LOGOUT_NOT_SIGNED_IN;
    if (strcmp(status, "UNR") == 0) return LOGOUT_NOT_REGISTERED;
    if (strcmp(status, "WRP") == 0) return LOGOUT_WRONG_PASSWORD;
    return LOGOUT_ERROR;
}

int client_unregister(connection_info cInfo, user_info uInfo){
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s\n", REQ_UNREGISTER, uInfo.UID, uInfo.password);

    if (sendUDP(cInfo.sockfd, buf, strlen(buf), 0, res) == SEND_UDP_ERROR) return REQUEST_NOT_SENT;

    memset(buf, 0, sizeof(buf));

    int received = recvUDP(cInfo.sockfd, buf, sizeof(buf), 0, &addr);
    if (received == RECV_TIMEOUT_ERROR) return DS_TIMEOUT;
    if (received == RECV_UDP_ERROR) return REPLY_NOT_RECEIVED;

    char* token = strtok(buf, " \n");
    if (token == NULL || strcmp(token, ANS_UNREGISTER)) return UNREGISTER_ERROR;

    char* status = strtok(NULL, " \n");
    if (status == NULL) return UNREGISTER_ERROR;

    if (strcmp(status, "OK") == 0) return UNREGISTER_SUCCESS;
    if (strcmp(status, "NOK") == 0) return UNREGISTER_NOT_SIGNED_IN;
    if (strcmp(status, "UNR") == 0) return UNREGISTER_NOT_REGISTERED;
    if (strcmp(status, "WRP") == 0) return UNREGISTER_WRONG_PASSWORD;
    return UNREGISTER_ERROR;
}

int client_publish(connection_info cInfo, user_info uInfo, file_info fInfo){
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s %s %zu %s\n", REQ_PUBLISH, uInfo.UID, uInfo.password, fInfo.filename, fInfo.filesize, fInfo.label);

    if (sendUDP(cInfo.sockfd, buf, strlen(buf), 0, res) == SEND_UDP_ERROR) return REQUEST_NOT_SENT;

    memset(buf, 0, sizeof(buf));

    int received = recvUDP(cInfo.sockfd, buf, sizeof(buf), 0, &addr);
    if (received == RECV_TIMEOUT_ERROR) return DS_TIMEOUT;
    if (received == RECV_UDP_ERROR) return REPLY_NOT_RECEIVED;

    char* token = strtok(buf, " \n");
    if (token == NULL || strcmp(token, ANS_PUBLISH)) return PUBLISH_ERROR;

    char* status = strtok(NULL, " \n");
    if (status == NULL) return PUBLISH_ERROR;

    if (strcmp(status, "OK") == 0) return PUBLISH_SUCCESS;
    if (strcmp(status, "NLG") == 0) return PUBLISH_NOT_SIGNED_IN;
    if (strcmp(status, "UNR") == 0) return PUBLISH_NOT_REGISTERED;
    if (strcmp(status, "WRP") == 0) return PUBLISH_WRONG_PASSWORD;
    if (strcmp(status, "NOK") == 0) return PUBLISH_FAILED;
    return PUBLISH_ERROR;
}

int client_remove_file(connection_info cInfo, user_info uInfo, char* filename){
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s %s\n", REQ_REMOVE_FILE, uInfo.UID, uInfo.password, filename);

    if (sendUDP(cInfo.sockfd, buf, strlen(buf), 0, res) == SEND_UDP_ERROR) return REQUEST_NOT_SENT;

    memset(buf, 0, sizeof(buf));

    int received = recvUDP(cInfo.sockfd, buf, sizeof(buf), 0, &addr);
    if (received == RECV_TIMEOUT_ERROR) return DS_TIMEOUT;
    if (received == RECV_UDP_ERROR) return REPLY_NOT_RECEIVED;

    char* token = strtok(buf, " \n");
    if (token == NULL || strcmp(token, ANS_REMOVE_FILE)) return REMOVE_FILE_ERROR;

    char* status = strtok(NULL, " \n");
    if (status == NULL) return REMOVE_FILE_ERROR;

    if (strcmp(status, "OK") == 0) return REMOVE_FILE_SUCCESS;
    if (strcmp(status, "NLG") == 0) return REMOVE_FILE_NOT_SIGNED_IN;
    if (strcmp(status, "UNR") == 0) return REMOVE_FILE_NOT_REGISTERED;
    if (strcmp(status, "WRP") == 0) return REMOVE_FILE_WRONG_PASSWORD;
    if (strcmp(status, "NOK") == 0) return REMOVE_FILE_FAILED;
    return REMOVE_FILE_ERROR;
}

int client_list_file_status(const char* reply){
    char prefix[8], status[8];
    if (sscanf(reply, "%7s %7s", prefix, status) != 2) return LIST_FILE_ERROR;
    if (strcmp(prefix, ANS_LIST_FILE)) return LIST_FILE_ERROR;

    if (strcmp(status, "OK") == 0) return LIST_FILE_SUCCESS;
    if (strcmp(status, "NOK") == 0) return LIST_FILE_NO_RESOURCES;
    return LIST_FILE_ERROR;
}

int client_list_file(connection_info cInfo, char** reply){
    *reply = NULL;

    char request[MAX_INSTRUCTION_LENGTH];
    snprintf(request, sizeof(request), "%s\n", REQ_LIST_FILE);

    if (sendUDP(cInfo.sockfd, request, strlen(request), 0, res) == SEND_UDP_ERROR) return REQUEST_NOT_SENT;
    
    char* buf = malloc(MAX_LIST_FILE_REPLY_LENGTH);
    if (buf == NULL) return LIST_FILE_ERROR;

    memset(buf, 0, MAX_LIST_FILE_REPLY_LENGTH);

    int received = recvUDP(cInfo.sockfd, buf, MAX_LIST_FILE_REPLY_LENGTH, 0, &addr);
    if (received == RECV_TIMEOUT_ERROR) {free(buf); return DS_TIMEOUT;}
    if (received == RECV_UDP_ERROR) {free(buf); return REPLY_NOT_RECEIVED;}

    int status = client_list_file_status(buf);

    if (status == LIST_FILE_SUCCESS) *reply = buf;

    else free(buf);

    return status;
}

static int sendTCP(int fd, const char* buf, size_t n){
    size_t sent = 0;
    while (sent < n){
        ssize_t written = write(fd, buf + sent, n - sent);
        if (written <= 0){
            printf(SEND_TCP_ERROR_MESSAGE);
            return SEND_TCP_ERROR;
        }
        sent += written;
    }
    return sent;
}

//Reads until the DS closes connection, grows buffer to accomodate longer messages
int recvTCP(int fd, char** reply){
    size_t capacity = INITIAL_TCP_BUFFER_SIZE;
    size_t total = 0;

    char* buf = malloc(capacity);
    if (buf == NULL){
        printf(ALLOCATION_ERROR_MESSAGE);
        return RECV_TCP_ERROR;
    }

    while (1){
        if (total + 1 >= capacity){
            capacity *= 2;
            char* grown = realloc(buf, capacity);
            if (grown == NULL){
                printf(ALLOCATION_ERROR_MESSAGE);
                free(buf);
                return RECV_TCP_ERROR;
            }
            buf = grown;
        }

        ssize_t bytesRead = read(fd, buf + total, capacity - 1 - total);
        if (bytesRead == 0) break; //DS closed the connection: the reply is complete
        if (bytesRead== -1){
            free(buf);
            if (errno == EAGAIN || errno == EWOULDBLOCK){
                printf(DS_TIMEOUT_MESSAGE);
                return RECV_TIMEOUT_ERROR;
            }
            printf(RECV_TCP_ERROR_MESSAGE);
            return RECV_TCP_ERROR;
        }
        total += bytesRead;
    }

    buf[total] = '\0';
    *reply = buf;
    return total;
}

int versions_status(const char* reply){
    char prefix[8], status[8];
    if (sscanf(reply, "%7s %7s", prefix, status) != 2) return VERSIONS_ERROR;
    if (strcmp(prefix, ANS_VERSIONS)) return VERSIONS_ERROR;

    if (strcmp(status, "OK") == 0) return VERSIONS_SUCCESS;
    if (strcmp(status, "NOK") == 0) return VERSIONS_NO_PEERS;
    return VERSIONS_ERROR;
}

int client_versions(connection_info cInfo, char* filename, char** reply){
    *reply = NULL;

    int fd = connectTCP(cInfo);
    if (fd == TCP_CONNECT_ERROR) return REQUEST_NOT_SENT;

    char request[MAX_INSTRUCTION_LENGTH];
    snprintf(request, sizeof(request), "%s %s\n", REQ_VERSIONS, filename);

    int status;
    if (sendTCP(fd, request, strlen(request)) == SEND_TCP_ERROR){
        status = REQUEST_NOT_SENT;
    }
    else {
        char* received = NULL;
        int response = recvTCP(fd, &received);

        if (response == RECV_TIMEOUT_ERROR) status = DS_TIMEOUT;
        else if (response == RECV_TCP_ERROR) status = REPLY_NOT_RECEIVED;
        else {
            status = versions_status(received);
            if (status == VERSIONS_SUCCESS) *reply = received;
            else free(received);
        }
    }

    disconnectTCP(fd);
    return status;
}