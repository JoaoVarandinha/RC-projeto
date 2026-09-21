#include "parser.h"
#include "commands.h"
#include "messages.h"
#include "validation.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

connection_info setup_client(int argc, char* argv[]){
    int opt;

    connection_info cInfo;
    cInfo.peerport = NULL;
    cInfo.dsip = OUTSIDE_DSIP;
    cInfo.dsport = DEFAULT_DSPORT;

    while ((opt = getopt(argc, argv, "m:n:p:")) != -1){
        switch (opt){
            case 'm': cInfo.peerport = optarg; break;
            case 'n': cInfo.dsip = optarg; break;
            case 'p': cInfo.dsport = optarg; break;
            default: break;
        }
    }

    if (cInfo.peerport == NULL){
        printf(PEERPORT_MISSING_MESSAGE);
        exit(-1);
    }

    if (!is_valid_PORT(cInfo.peerport)){
        printf(INVALID_PEERPORT_MESSAGE);
        exit(-1);
    }

    if (!is_valid_IP(cInfo.dsip)){
        printf(INVALID_DSIP_MESSAGE);
        exit(-1);
    }

    if (!is_valid_PORT(cInfo.dsport)){
        printf(INVALID_DSPORT_MESSAGE);
        exit(-1);
    }
    return cInfo;
}

void read_commands(connection_info cInfo){
    char buf[MAX_INSTRUCTION_LENGTH];

    while (fgets(buf, sizeof(buf), stdin)){
        if(process_command(cInfo, buf)==EXIT_CODE) break;
    }
}
