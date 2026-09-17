#ifndef COMMANDS_H
#define COMMANDS_H

void process_command(int fd, char* command);

void login(int fd, char* command);

void logout(int fd);

void unregister(int fd);

void exit();

#endif