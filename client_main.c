#include "api.h"
#include "parser.h"
#include "client_main.h"


int main(int argc, char* argv[]){
    connection_info cInfo = setup_client(argc, argv);
    connectUDP(&cInfo);
    read_commands(cInfo);
    return 0;
}