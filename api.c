#include "api.h"
#include "parser.h"
#include "client_main.h"
#include "commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>

int fd, errcode;
struct addrinfo *res;
struct sockaddr_in addr;

void connectUDP(connection_info *cInfo){

    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1) exit(1);
    cInfo->sockfd = fd;

    struct timeval timeout;
    timeout.tv_sec = RECV_TIMEOUT;
    timeout.tv_usec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) exit(1);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo(cInfo->dsip, cInfo->dsport, &hints, &res);
    if (errcode != 0) exit(1);
}

void disconnectUDP(){
    freeaddrinfo(res);
    close(fd);
}

void sendUDP(int fd, const void* buf, size_t n, int flags, struct addrinfo* res){
    size_t s = sendto(fd, buf, n, flags, res->ai_addr, res->ai_addrlen);
    if (s == -1){
        exit(1);
    }
}

int recvUDP(int fd, void* buf, size_t n, int flags, struct sockaddr_in* addr){
    socklen_t addrlen = sizeof(struct sockaddr_in);
    ssize_t s = recvfrom(fd, buf, n, flags, (struct sockaddr*) addr, &addrlen);
    if (s == -1){
        if (errno == EAGAIN || errno == EWOULDBLOCK){
            return RECV_ERROR; // DS did not reply within the timeout
        }
        exit(1);
    }
    return s;
}

int client_login(connection_info cInfo, user_info uInfo){
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s %s\n", REQ_LOGIN, uInfo.UID, uInfo.password, cInfo.peerport);

    sendUDP(cInfo.sockfd, buf, strlen(buf), 0, res);

    memset(buf, 0, sizeof(buf));

    if (recvUDP(cInfo.sockfd, buf, sizeof(buf), 0, &addr) == RECV_ERROR) return DS_TIMEOUT;

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

    sendUDP(cInfo.sockfd, buf, strlen(buf), 0, res);

    memset(buf, 0, sizeof(buf));

    if (recvUDP(cInfo.sockfd, buf, sizeof(buf), 0, &addr) == RECV_ERROR) return DS_TIMEOUT;

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

    sendUDP(cInfo.sockfd, buf, strlen(buf), 0, res);

    memset(buf, 0, sizeof(buf));

    if (recvUDP(cInfo.sockfd, buf, sizeof(buf), 0, &addr) == RECV_ERROR) return DS_TIMEOUT;

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

    sendUDP(cInfo.sockfd, buf, strlen(buf), 0, res);

    memset(buf, 0, sizeof(buf));

    if (recvUDP(cInfo.sockfd, buf, sizeof(buf), 0, &addr) == RECV_ERROR) return DS_TIMEOUT;

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

    sendUDP(cInfo.sockfd, buf, strlen(buf), 0, res);

    memset(buf, 0, sizeof(buf));

    if (recvUDP(cInfo.sockfd, buf, sizeof(buf), 0, &addr) == RECV_ERROR) return DS_TIMEOUT;

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