#ifndef PARSER_H
#define PARSER_H

#define MAX_INSTRUCTION_LENGTH 128

int is_valid_PORT(char* PORT);

int is_valid_UID(char* UID);

int is_valid_IP(char* ID);

int is_valid_password(char* password);

void process_command(int fd, char* command);

#endif