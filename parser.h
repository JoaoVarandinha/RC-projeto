#ifndef PARSER_H
#define PARSER_H

#include "client_main.h"
#include <stdbool.h>

#define MAX_INSTRUCTION_LENGTH 128

bool is_valid_PORT(char* PORT);

bool is_valid_UID(char* UID);

bool is_valid_IP(char* ID);

bool is_valid_password(char* password);

void read_commands(connection_info cInfo);

connection_info setup_client(int argc, char* argv[]);

#endif