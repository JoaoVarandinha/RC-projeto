#include "parser.h"
#include "commands.h"
#include "messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>

bool is_valid_PORT(char* PORT) {
    if (PORT == NULL) {
        return false;
    }
    //Check length (ports are 1 to 5 digits: 1-65535)
    size_t len = strlen(PORT);
    if (len < 1 || len > 5) {
        return false;
    }
    //Check if all char are digits
    for (size_t i = 0; i < len; i++) {
        if (!isdigit(PORT[i])) {
            return false;
        }
    }
    //Check is number is an existing port
    int PORT_NUM = atoi(PORT);
    if (PORT_NUM < 1 || PORT_NUM > 65535) {
        return false;
    }
    return true;
}

bool is_valid_UID(char* UID) {
    if (UID == NULL) {
        return false;
    }
    //Check length
    if (strlen(UID) != 6) {
        return false;
    }
    //Check if all char are digits
    for (int i = 0; i < 6; i++) {
        if (!isdigit(UID[i])) {
            return false;
        }
    }
    return true;
}

bool is_valid_IP(char* IP) {
    if (IP == NULL) {
        return false;
    }
    struct in_addr addr;
    return inet_pton(AF_INET, IP, &addr) == 1;
}

bool is_valid_password(char* password) {
    if (password == NULL) {
        return false;
    }
    //Check length
    if (strlen(password) != 8) {
        return false;
    }
    //Check if all char are letters or digits
    for (int i = 0; i < 8; i++) {
        if (!isalnum(password[i])) {
            return false;
        }
    }
    return true;
}

connection_info setup_client(int argc, char* argv[]) {
    int opt;

    connection_info info;
    info.peerport = NULL;
    info.dsip = OUTSIDE_DSIP;
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
        printf(PEERPORT_MISSING_MESSAGE);
        exit(-1);
    }

    if (!is_valid_PORT(info.peerport)) {
        printf(INVALID_PEERPORT_MESSAGE);
        exit(-1);
    }

    if (!is_valid_IP(info.dsip)) {
        printf(INVALID_DSIP_MESSAGE);
        exit(-1);
    }

    if (!is_valid_PORT(info.dsport)) {
        printf(INVALID_DSPORT_MESSAGE);
        exit(-1);
    }
    return info;
}

void read_commands(connection_info cInfo) {
    char buf[MAX_INSTRUCTION_LENGTH];

    while (fgets(buf, sizeof(buf), stdin)) {
        if(process_command(cInfo, buf)==EXIT_CODE) break;
    }
}
