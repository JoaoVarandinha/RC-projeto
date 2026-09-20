#ifndef COMMANDS_H
#define COMMANDS_H

#include "client_main.h"
#include "api.h"
#include "parser.h"
#include <stdbool.h>

#define EXIT_CODE -1

int process_command(connection_info cInfo, char* command);

void login(connection_info cInfo, char* UID, char* password);

void logout(connection_info cInfo);

void unregister(connection_info cInfo);

bool isLoggedIn();

#endif