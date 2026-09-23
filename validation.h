#ifndef VALIDATION_H
#define VALIDATION_H

#include "client_main.h"
#include <stdbool.h>

#define allowedChars {'-','_','.'}

bool isLoggedIn(user_info uInfo);

bool is_valid_PORT(char* PORT);

bool is_valid_UID(char* UID);

bool is_valid_IP(char* IP);

bool is_valid_password(char* password);

bool is_valid_filename(char* filename);

bool is_valid_label(char* label);

bool is_valid_Fsize(char* Fsize);

bool is_valid_version_reply(char* UID, char* Fsize, char* label, char* pubTime, char* availability);

//COMMAND INPUT VALIDATION

bool is_valid_login(user_info uInfo, char* UID, char* password);

bool is_valid_logout(user_info uInfo);

bool is_valid_unregister(user_info uInfo);

bool is_valid_publish(user_info uInfo, char* filename, char* label);

bool is_valid_versions(char* filename);

#endif
