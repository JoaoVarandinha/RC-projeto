#include "api.h"
#include "parser.h"
#include "client_main.c"

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define LOGIN_SUCCESS 0
#define LOGIN_WRONG_PASSWORD 1
#define LOGIN_NEW_USER 2

#define LOGOUT_SUCCESS 0
#define LOGOUT_NOT_SIGNED_IN 1
#define LOGOUT_NOT_REGISTERED 2
#define LOGOUT_WRONG_PASSWORD 3

int fd, errcode;
struct addrinfo *res;
struct sockaddr_in addr;


int connectUPD(client_info info) {

    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1) exit(1);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo(info.dsip, info.dsport, &hints, &res);
    if (errcode != 0) exit(1);
}

int disconnectUPD() {
    freeaddrinfo(res);
    close(fd);
}

int sendUDP(int fd, const void* buf, size_t n, int flags, struct addrinfo* res) {
    size_t s = sendto(fd, buf, n, flags, res->ai_addr, res->ai_addrlen);
    if (s == -1) {
        exit(1);
    }
}

int recvUDP(int fd, void* buf, size_t n, int flags, struct sockaddr_in* addr) {
    int addrlen = sizeof(addr);
    size_t s = recvfrom(fd, buf, n, flags, (struct sockaddr*) &addr, &addrlen);
        if (s == -1) {
        exit(1);
    }
}

int client_login(client_info info) {
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s %s\n", REQ_LOGIN, info.user.UID, info.user.password, info.peerport);

    sendUDP(fd, buf, strlen(buf), 0, res);

    memset(buf, 0, sizeof(buf));

    recvUDP(fd, buf, sizeof(buf), 0, &addr);

    switch(buf[0]) {
        case 'O': return LOGIN_SUCCESS;
        case 'N': return LOGIN_WRONG_PASSWORD;
        case 'R': return LOGIN_NEW_USER;
    }
}

int client_logout(client_info info) {
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s\n", REQ_LOGOUT, info.user.UID, info.user.password);

    sendUDP(fd, buf, strlen(buf), 0, res);
    
    memset(buf, 0, sizeof(buf));

    recvUDP(fd, buf, sizeof(buf), 0, &addr);

    switch(buf[0]) {
        case 'O': return LOGOUT_SUCCESS;
        case 'N': return LOGOUT_NOT_SIGNED_IN;
        case 'U': return LOGOUT_NOT_REGISTERED;
        case 'W': return LOGOUT_WRONG_PASSWORD;
    }
}

//MISSING STATUS MESSAGES FOR THIS ONE
int client_unregister(client_info info) {
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s\n", REQ_UNREGISTER, info.user.UID, info.user.password);

    sendUDP(fd, buf, strlen(buf), 0, res);

    memset(buf, 0, sizeof(buf));

    recvUDP(fd, buf, sizeof(buf), 0, &addr);
}