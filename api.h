#ifndef API_H
#define API_H

#include "client_main.h"
#include <stddef.h>
#include <netdb.h>
#include <netinet/in.h>

#define UDP_CONNECT_ERROR -1
#define SEND_UDP_ERROR -1

#define RECV_TIMEOUT 5
#define RECV_TIMEOUT_ERROR -1
#define RECV_UDP_ERROR -2
#define SEND_TCP_ERROR -1
#define RECV_TCP_ERROR -2

//Returned by any client_* call when the DS did not reply in time.
//Shared across all commands, so it must not collide with their status codes.
#define DS_TIMEOUT -2

//Returned by any client_* call when the request never left the machine.
//sendUDP already told the user, so callers should stay silent about it.
#define REQUEST_NOT_SENT -3

//Returned by any client_* call when the request went out but the reply could
//not be read. recvUDP already told the user, so callers stay silent about it.
#define REPLY_NOT_RECEIVED -4

//Returned by connectTCP when the connection could not be established
#define TCP_CONNECT_ERROR -1


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

//REMOVE CODES
#define REMOVE_FILE_ERROR -1
#define REMOVE_FILE_SUCCESS 0
#define REMOVE_FILE_NOT_SIGNED_IN 1
#define REMOVE_FILE_NOT_REGISTERED 2
#define REMOVE_FILE_WRONG_PASSWORD 3
#define REMOVE_FILE_FAILED 4

//VERSIONS CODES
#define VERSIONS_ERROR -1
#define VERSIONS_SUCCESS 0
#define VERSIONS_NO_PEERS 1

//Starting size of the RVR buffer
#define INITIAL_TCP_BUFFER_SIZE 8192

//CLIENT REQUESTS
#define REQ_LOGIN "LIN"
#define REQ_LOGOUT "LOU"
#define REQ_UNREGISTER "UNR"
#define REQ_PUBLISH "PUB"
#define REQ_REMOVE_FILE "REM"
#define REQ_VERSIONS "VRS"

//SERVER ANSWERS
#define ANS_LOGIN "RLI"
#define ANS_LOGOUT "RLO"
#define ANS_UNREGISTER "RUR"
#define ANS_PUBLISH "RPB"
#define ANS_REMOVE_FILE "RRM"
#define ANS_VERSIONS "RVR"

//Availability field of an RVR entry
#define AVAILABLE "AVL"
#define NOT_AVAILABLE "NAV"

int connectUDP(connection_info *cInfo);

void disconnectUDP(int sockfd);

int connectTCP(connection_info cInfo);

void disconnectTCP(int sockfd);

int sendUDP(int sockfd, const void* buf, size_t n, int flags, struct addrinfo* res);

int recvUDP(int sockfd, void* buf, size_t n, int flags, struct sockaddr_in* addr);

int client_login(connection_info cInfo, user_info uInfo);

int client_logout(connection_info cInfo, user_info uInfo);

int client_unregister(connection_info cInfo, user_info uInfo);

int client_publish(connection_info cInfo, user_info uInfo, file_info fInfo);

int client_remove_file(connection_info cInfo, user_info uInfo, char* filename);

int client_versions(connection_info cInfo, char* filename, char** reply);

#endif