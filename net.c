#include "net.h"
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

//DS address resolved once by connectUDP and used by every sendUDP
static struct addrinfo *res;
//Filled in with the sender of each datagram recvUDP receives
static struct sockaddr_in addr;

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

int sendUDP(int fd, const void* buf, size_t n){
    ssize_t s = sendto(fd, buf, n, 0, res->ai_addr, res->ai_addrlen);
    if (s == -1){
        printf(SEND_UDP_ERROR_MESSAGE);
        return SEND_UDP_ERROR;
    }
    return s;
}

int recvUDP(int fd, void* buf, size_t n){
    socklen_t addrlen = sizeof(struct sockaddr_in);
    for (int i = RECV_UDP_TRIES; i > 0; i--){
        ssize_t s = recvfrom(fd, buf, n, 0, (struct sockaddr*) &addr, &addrlen);
        if (s == -1){
            if (errno == EAGAIN || errno == EWOULDBLOCK){
                printf(DS_TIMEOUT_RETRY_MESSAGE);
                continue;
            }
            printf(RECV_UDP_ERROR_MESSAGE);
            return RECV_UDP_ERROR;
        }
        return s;
    }
    printf(DS_TIMEOUT_MESSAGE);
    return RECV_TIMEOUT_ERROR;
}


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

int sendTCP(int fd, const char* buf, size_t n){
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
            //realloc leaves the original block valid when it fails, so the new
            //pointer is kept separate to avoid losing it and leaking
            char* grown = realloc(buf, capacity);
            if (grown == NULL){
                printf(ALLOCATION_ERROR_MESSAGE);
                free(buf);
                return RECV_TCP_ERROR;
            }
            buf = grown;
        }

        ssize_t bytesRead = read(fd, buf + total, capacity - 1 - total);
        if (bytesRead == 0) break; //DS closed before sending the terminator
        if (bytesRead == -1){
            free(buf);
            if (errno == EAGAIN || errno == EWOULDBLOCK){
                printf(DS_TIMEOUT_MESSAGE);
                return RECV_TIMEOUT_ERROR;
            }
            printf(RECV_TCP_ERROR_MESSAGE);
            return RECV_TCP_ERROR;
        }
        total += bytesRead;

        //A partial read just loops and waits for the rest, so the \n is only
        //seen once the whole message has arrived
        if (buf[total - 1] == '\n') break;
    }

    buf[total] = '\0';
    *reply = buf;
    return total;
}
