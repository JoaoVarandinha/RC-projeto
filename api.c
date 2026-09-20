#include "api.h"
#include "parser.h"
#include "client_main.h"

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

void connectUDP(connection_info *info) {

    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1) exit(1);
    info->sockfd = fd;

    struct timeval timeout;
    timeout.tv_sec = RECV_TIMEOUT;
    timeout.tv_usec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) exit(1);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo(info->dsip, info->dsport, &hints, &res);
    if (errcode != 0) exit(1);
}

void disconnectUDP() {
    freeaddrinfo(res);
    close(fd);
}

void sendUDP(int fd, const void* buf, size_t n, int flags, struct addrinfo* res) {
    size_t s = sendto(fd, buf, n, flags, res->ai_addr, res->ai_addrlen);
    if (s == -1) {
        exit(1);
    }
}

int recvUDP(int fd, void* buf, size_t n, int flags, struct sockaddr_in* addr) {
    socklen_t addrlen = sizeof(struct sockaddr_in);
    ssize_t s = recvfrom(fd, buf, n, flags, (struct sockaddr*) addr, &addrlen);
    if (s == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return RECV_ERROR; // DS did not reply within the timeout
        }
        exit(1);
    }
    return s;
}

int client_login(connection_info info, user_info user) {
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s %s\n", REQ_LOGIN, user.UID, user.password, info.peerport);

    sendUDP(info.sockfd, buf, strlen(buf), 0, res);

    memset(buf, 0, sizeof(buf));

    if (recvUDP(info.sockfd, buf, sizeof(buf), 0, &addr) == RECV_ERROR) return LOGIN_ERROR;

    char* token = strtok(buf," ");
    if (token == NULL || strcmp(token,ANS_LOGIN)) return LOGIN_ERROR;

    char* status = strtok(NULL," ");
    if (status == NULL) return LOGIN_ERROR;

    switch(status[0]) {
        case 'O': return LOGIN_SUCCESS;
        case 'N': return LOGIN_WRONG_PASSWORD;
        case 'R': return LOGIN_NEW_USER;
        default: return LOGIN_ERROR;
    }
}

int client_logout(connection_info info, user_info user) {
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s\n", REQ_LOGOUT, user.UID, user.password);

    sendUDP(info.sockfd, buf, strlen(buf), 0, res);

    memset(buf, 0, sizeof(buf));

    if (recvUDP(info.sockfd, buf, sizeof(buf), 0, &addr) == RECV_ERROR) return LOGOUT_ERROR;

    char* token = strtok(buf, " ");
    if (token == NULL || strcmp(token, ANS_LOGOUT)) return LOGOUT_ERROR;

    char* status = strtok(NULL, " ");
    if (status == NULL) return LOGOUT_ERROR;

    switch(status[0]) {
        case 'O': return LOGOUT_SUCCESS;
        case 'N': return LOGOUT_NOT_SIGNED_IN;
        case 'U': return LOGOUT_NOT_REGISTERED;
        case 'W': return LOGOUT_WRONG_PASSWORD;
        default: return LOGOUT_ERROR;
    }
}

int client_unregister(connection_info info, user_info user) {
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s\n", REQ_UNREGISTER, user.UID, user.password);

    sendUDP(info.sockfd, buf, strlen(buf), 0, res);

    memset(buf, 0, sizeof(buf));

    if (recvUDP(info.sockfd, buf, sizeof(buf), 0, &addr) == RECV_ERROR) return UNREGISTER_ERROR;

    char* token = strtok(buf, " ");
    if (token == NULL || strcmp(token, ANS_UNREGISTER)) return UNREGISTER_ERROR;

    char* status = strtok(NULL, " ");
    if (status == NULL) return UNREGISTER_ERROR;

    switch(status[0]) {
        case 'O': return UNREGISTER_SUCCESS;
        case 'N': return UNREGISTER_NOT_SIGNED_IN;
        case 'U': return UNREGISTER_NOT_REGISTERED;
        case 'W': return UNREGISTER_WRONG_PASSWORD;
        default: return UNREGISTER_ERROR;
    }
}