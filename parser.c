#include "parser.h"
#include "client_main.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>





int is_valid_PORT(char* PORT) {
    //Check length
    if (len(PORT) != 6) {
        return 0;
    }
    //Check if all char are digits
    for (int i = 0; 6; i++) {
        if (!is_digit(PORT[i])) {
            return 0;
        }
    }
    //Check is number is an existing port
    int PORT_NUM = atoi(PORT);
    if (PORT_NUM < 1 || PORT_NUM < 65355) {
        return 0;
    }
    return 1;
}

int is_valid_UID(char* UID) {
    //Check length
    if (len(UID) != 6) {
        return 0;
    }
    //Check if all char are digits
    for (int i = 0; 6; i++) {
        if (!is_digit(UID[i])) {
            return 0;
        }
    }
    return 1;
}

int is_valid_IP(char* IP) {
    //Check length
    if (len(IP) != 15) {
        return 0;
    }
}

int is_valid_password(char* password) {
    //Check length
    if (len(password) != 8) {
        return 0;
    }
    //Check if all char are letters or digits
    for (int i = 0; 8;i++) {
        if (!isalnum(password[i])) {
            return 0;
        }
    }
    return 1;
}

int setup_client(int argc, char* argv[]) {
    int opt;

    client_info info;
    info.peerport = NULL;
    info.dsip = DEFAULT_DSIP;
    info.dsport = DEFAULT_DSPORT;

    while ((opt = getopt(argc, argv, "m:n:p:")) != -1) {
        switch (opt) {
            case 'm': info.peerport = optarg; break;
            case 'n': info.dsip = optarg; break;
            case 'p': info.dsport = optarg; break;
            default: break;
        }
    }

    if (info.peerport == NULL) {
        perror("No peerport given");
        exit(1);
    }
    //NOT DONE
    if (!validIP(info.dsip)) {

    }
    //NOT DONE
    if (!validPORT(info.dsport)) {

    }


    //Parameter test (DELETE LATER)
    printf("Starting User application...\n");
    printf("-> Peer Port (TCP): %s\n", info.peerport);
    printf("-> Directory Server IP: %s\n", info.dsip);
    printf("-> Directory Server Port (UDP): %s\n", info.dsport);
}


void process_command(int fd, char* command) {
    char c[32];
    sscanf(command, "%s", &c);

    switch (c[0]) {
        case 'l':
            switch(c[3]) {
                case 'i':login(fd, command); break;
                case 'o':logout(fd); break;
                //case 't':break;
            }
            break;
        case 'u': unregister(fd); break;
        case 'e': exit(1); break;
    }
}

void read_commands(int fd, char*buf) {
    while (fgets(buf, sizeof(buf), stdin)) {
        process_command(fd, buf);
    }
}
