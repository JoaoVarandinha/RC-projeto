#ifndef API_H
#define API_H

#include "client_main.c"

//CLIENT REQUESTS
#define REQ_LOGIN "LIN"
#define REQ_LOGOUT "LOU"
#define REQ_UNREGISTER "UNR"

//SERVER ANSWERS
#define ANS_LOGIN "RLI"
#define ANS_LOGOUT "RLO"
#define ANS_UNREGISTER "RUR"

int connectUPD(client_info info);

int disconnectUPD();

int sendUDP(int fd, const void* buf, size_t n, int flags, struct addrinfo* res);

int recvUDP(int fd, void* buf, size_t n, int flags, struct sockaddr_in* addr);

int client_login(client_info info);

int client_logout(client_info info);

int client_unregister(client_info info);

#endif