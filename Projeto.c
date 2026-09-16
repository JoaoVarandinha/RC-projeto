#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DEFAULT_HOSTNAME "tejo.tecnico.ulisboa.pt" //Delete if uneeded
#define DEFAULT_DSIP "192.168.1.1"
#define OUTSIDE_DSIP "193.136.138.142"
#define DEFAULT_DSPORT "59000"


//REQUESTS
//#define

//ACKNOWLEDGMENTS

//ERRORS

void sendUDP(int fd, const void* buf, size_t n, int flags, struct addrinfo* res) {
    size_t s = sendto(fd, buf, n, flags, res->ai_addr, res->ai_addrlen);
    if (s == -1) {
        perror("SendUDP failed");
        exit(1);
    }
}

void recvUDP(int fd, void* buf, size_t n, int flags, struct sockaddr_in* addr) {
    int addrlen = sizeof(addr);
    size_t s = recvfrom(fd, buf, n, flags, (struct sockaddr*) &addr, &addrlen);
        if (s == -1) {
        perror("RecvUDP failed");
        exit(1);
    }
}

//NOT DONE
int validPORT(int PORT) {

}

int validUID(char* UID) {
    //Check length
    if (len(UID) != 6) {
        return 0;
    }
    //Check if all char are digits
    for (int i = 0; 6;i++) {
        if (!is_digit(UID[i])) {
            return 0;
        }
    }
    return 1;
}

int validIP(char* IP) {
    //Check length
    if (len(IP) != 8) {
        return 0;
    }
    //Check if all char are letters or digits
    for (int i = 0; 8;i++) {
        if (!isalnum(IP[i])) {
            return 0;
        }
    }
}


void read_command(char* command) {

    char c[32];
    sscanf(command, "%s", &c);

    switch (c[0]) {
        case 'l':
            switch(c[3]) {
                case 'i':login(command); break;
                case 'o':break;
                case 't':break;
            }
            break;
        case 'u': unregister(); break;
        case 'e': exit(); break;
    }
}

void login() {

}

void logout() {

}

void unregister() {

}

void exit() {

}


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
        perror("No peerport given");
        exit(1);
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
    char buf[128];

    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1) exit(1);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo(dsip, dsport, &hints, &res);
    if (errcode != 0) exit(1);




    freeaddrinfo(res);
    close(fd);
    return 0;
}

