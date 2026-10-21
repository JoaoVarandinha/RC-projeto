#ifndef NET_H
#define NET_H

#include "client_main.h"
#include <stddef.h>

//Seconds a receive waits before giving up
#define RECV_TIMEOUT 5
//How many receive timeouts recvUDP waits through before giving up on a reply
#define RECV_UDP_TRIES 3

//Starting size of the buffer recvTCP grows as the reply arrives
#define INITIAL_TCP_BUFFER_SIZE 8192

#define UDP_CONNECT_ERROR -1
#define TCP_CONNECT_ERROR -1
#define SEND_UDP_ERROR -1
#define SEND_TCP_ERROR -1
#define RECV_TIMEOUT_ERROR -1
#define RECV_UDP_ERROR -2
#define RECV_TCP_ERROR -2

//Every function below reports its own failures to the user, so callers only
//need to act on the returned code, not print anything about it

//Opens the UDP socket used for every DS request and resolves the DS address,
//which sendUDP then sends to
int connectUDP(connection_info *cInfo);

void disconnectUDP(int sockfd);

//Opens a TCP connection to the DS, which listens for TCP on the same port
//number it uses for UDP. Returns the connected socket or TCP_CONNECT_ERROR.
int connectTCP(connection_info cInfo);

void disconnectTCP(int sockfd);

int sendUDP(int sockfd, const void* buf, size_t n);

int recvUDP(int sockfd, void* buf, size_t n);

//Writes all n bytes, looping because TCP may accept fewer than asked
int sendTCP(int sockfd, const char* buf, size_t n);

//Reads one \n terminated message into a heap buffer the CALLER MUST free.
//Not suitable for binary data, which may contain \n before its end.
int recvTCP(int sockfd, char** reply);

#endif
