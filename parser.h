#ifndef PARSER_H
#define PARSER_H

#include "client_main.h"

#define MAX_INSTRUCTION_LENGTH 128

void read_commands(connection_info cInfo);

connection_info setup_client(int argc, char* argv[]);

#endif