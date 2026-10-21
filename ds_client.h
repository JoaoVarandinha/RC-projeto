#ifndef DS_CLIENT_H
#define DS_CLIENT_H

#include "client_main.h"
#include <stddef.h>

//Returned by any client_* call when the DS did not reply in time.
//Shared across all commands, so it must not collide with their status codes.
#define DS_TIMEOUT -2

//Returned by any client_* call when the request never left the machine.
//net.c already told the user, so callers should stay silent about it.
#define REQUEST_NOT_SENT -3

//Returned by any client_* call when the request went out but the reply could
//not be read. net.c already told the user, so callers stay silent about it.
#define REPLY_NOT_RECEIVED -4

//LOGIN CODES
#define LOGIN_ERROR -1
#define LOGIN_SUCCESS 0
#define LOGIN_WRONG_PASSWORD 1
#define LOGIN_NEW_USER 2

//LOGOUT CODES
#define LOGOUT_ERROR -1
#define LOGOUT_SUCCESS 0
#define LOGOUT_NOT_SIGNED_IN 1
#define LOGOUT_NOT_REGISTERED 2
#define LOGOUT_WRONG_PASSWORD 3

//UNREGISTER CODES
#define UNREGISTER_ERROR -1
#define UNREGISTER_SUCCESS 0
#define UNREGISTER_NOT_SIGNED_IN 1
#define UNREGISTER_NOT_REGISTERED 2
#define UNREGISTER_WRONG_PASSWORD 3

//PUBLISH CODES
#define PUBLISH_ERROR -1
#define PUBLISH_SUCCESS 0
#define PUBLISH_NOT_SIGNED_IN 1
#define PUBLISH_NOT_REGISTERED 2
#define PUBLISH_WRONG_PASSWORD 3
#define PUBLISH_FAILED 4

//REMOVE CODES
#define REMOVE_FILE_ERROR -1
#define REMOVE_FILE_SUCCESS 0
#define REMOVE_FILE_NOT_SIGNED_IN 1
#define REMOVE_FILE_NOT_REGISTERED 2
#define REMOVE_FILE_WRONG_PASSWORD 3
#define REMOVE_FILE_FAILED 4

//LIST CODES
#define LIST_FILE_ERROR -1
#define LIST_FILE_SUCCESS 0
#define LIST_FILE_NO_RESOURCES 1

//VERSIONS CODES
#define VERSIONS_ERROR -1
#define VERSIONS_SUCCESS 0
#define VERSIONS_NO_PEERS 1

int client_login(connection_info cInfo, user_info uInfo);

int client_logout(connection_info cInfo, user_info uInfo);

int client_unregister(connection_info cInfo, user_info uInfo);

int client_publish(connection_info cInfo, user_info uInfo, file_info fInfo);

int client_remove_file(connection_info cInfo, user_info uInfo, char* filename);

//On LIST_FILE_SUCCESS, *reply points to a heap buffer holding the whole RLS
//reply, which the CALLER MUST free. On every other return *reply is NULL.
int client_list_file(connection_info cInfo, char** reply);

//On VERSIONS_SUCCESS, *reply points to a heap buffer holding the whole RVR
//reply, which the CALLER MUST free. On every other return *reply is NULL.
int client_versions(connection_info cInfo, char* filename, char** reply);

#endif
