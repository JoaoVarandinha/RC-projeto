#ifndef API_H
#define API_H

#include "client_main.h"
#include <stddef.h>
#include <netdb.h>
#include <netinet/in.h>

#define RECV_TIMEOUT 5
#define RECV_ERROR -1

//Returned by any client_* call when the DS did not reply in time.
//Shared across all commands, so it must not collide with their status codes.
#define DS_TIMEOUT -2

//LOGIN CODES
#define LOGIN_ERROR -1
#define LOGIN_SUCCESS 0
#define LOGIN_WRONG_PASSWORD 1
#define LOGIN_NEW_USER 2

//LOGOUT CODES
#define LOGOUT_ERROR -1
#define LOGOUT_SUCCESS 0
#define LOGOUT_NOT_SIGNED_IN 1
#define LOGOUT_NOT_REGISTERED 2
#define LOGOUT_WRONG_PASSWORD 3

//UNREGISTER CODES
#define UNREGISTER_ERROR -1
#define UNREGISTER_SUCCESS 0
#define UNREGISTER_NOT_SIGNED_IN 1
#define UNREGISTER_NOT_REGISTERED 2
#define UNREGISTER_WRONG_PASSWORD 3

//PUBLISH CODES
#define PUBLISH_ERROR -1
#define PUBLISH_SUCCESS 0
#define PUBLISH_NOT_SIGNED_IN 1
#define PUBLISH_NOT_REGISTERED 2
#define PUBLISH_WRONG_PASSWORD 3
#define PUBLISH_FAILED 4

//CLIENT REQUESTS
#define REQ_LOGIN "LIN"
#define REQ_LOGOUT "LOU"
#define REQ_UNREGISTER "UNR"
#define REQ_PUBLISH "PUB"

//SERVER ANSWERS
#define ANS_LOGIN "RLI"
#define ANS_LOGOUT "RLO"
#define ANS_UNREGISTER "RUR"
#define ANS_PUBLISH "RPB"

void connectUDP(connection_info *cInfo);

void disconnectUDP();

void sendUDP(int sockfd, const void* buf, size_t n, int flags, struct addrinfo* res);

int recvUDP(int sockfd, void* buf, size_t n, int flags, struct sockaddr_in* addr);

int client_login(connection_info cInfo, user_info uInfo);

int client_logout(connection_info cInfo, user_info uInfo);

int client_unregister(connection_info cInfo, user_info uInfo);

int client_publish(connection_info cInfo, user_info uInfo, file_info fInfo);

#endif