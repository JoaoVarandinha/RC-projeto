#include "api.h"
#include "parser.h"
#include "client_main.h"


int main(int argc, char* argv[]){
    connection_info cInfo = setup_client(argc, argv);
    if (connectUDP(&cInfo) == UDP_CONNECT_ERROR) return -1;
    read_commands(cInfo);
    disconnectUDP(cInfo.sockfd);
    return 0;
}