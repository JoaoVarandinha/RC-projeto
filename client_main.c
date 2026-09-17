#include "api.h"
#include "parser.h"
#include "client_main.h"


void main(int argc, char* argv[]) {
    setup_client(argc, argv);
    connect_server();
    read_commands();
}