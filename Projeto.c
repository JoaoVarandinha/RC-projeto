#include <stdio.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define DEFAULT_DSIP "192.168.1.1"
#define OUTSIDE_DSIP "193.136.138.142"
#define DEFAULT_DSPORT "59000"

int main(int argc, char* argv[]) {
    int opt;
    char *peerport = NULL;
    char *dsip = DEFAULT_DSIP;
    char *dsport = DEFAULT_DSPORT;

    while ((opt = getopt(argc, argv, "m:n:p:")) != -1) {
        switch (opt) {
            case 'm': peerport = optarg; break;
            case 'n': dsip = optarg; break;
            case 'p': dsport = optarg; break;
            default: break;
        }
    }

    if (peerport == NULL) {
        //give error
    }


    //Parameter test (DELETE LATER)
    printf("Starting User application...\n");
    printf("-> Peer Port (TCP): %s\n", peerport);
    printf("-> Directory Server IP: %s\n", dsip);
    printf("-> Directory Server Port (UDP): %s\n", dsport);


    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];

    return 0;
}

