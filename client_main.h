#ifndef CLIENT_MAIN_H
#define CLIENT_MAIN_H

#define DEFAULT_HOSTNAME "tejo.tecnico.ulisboa.pt" //Delete if uneeded
#define DEFAULT_DSIP "192.168.1.1"
#define OUTSIDE_DSIP "193.136.138.142"
#define DEFAULT_DSPORT "59000"

struct {
    char UID[7];
    char password[9];
} typedef user_info;

struct {
    char* peerport;
    char* dsip;
    char* dsport;
    int sockfd;
} typedef connection_info;

#endif