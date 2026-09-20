#include "commands.h"
#include "messages.h"

#include <stdio.h>
#include <string.h>

user_info info;

int process_command(connection_info cInfo, char* command) {
    char *cmdToken = strtok(command, " \n");
    if (cmdToken == NULL) return 0;

    if (strcmp(cmdToken, "login") == 0) {
        char* UID = strtok(NULL, " \n");
        char* password = strtok(NULL, " \n");
        login(cInfo, UID, password);
    }

    else if (strcmp(cmdToken, "logout") == 0) {
        logout(cInfo);
    }

    else if (strcmp(cmdToken, "unregister") == 0) {
        unregister(cInfo);
    }

    else if (strcmp(cmdToken, "exit") == 0){
        if (isLoggedIn()) logout(cInfo);
        return EXIT_CODE;
    }

    else {
        printf(UNKNOWN_COMMAND_MESSAGE, command);
    }
    return 0;
}

void login(connection_info cInfo, char* UID, char* password) {
    if (!is_valid_UID(UID)) {
        printf(INVALID_UID_MESSAGE);
        return;
    }
    if (!is_valid_password(password)) {
        printf(INVALID_PASSWORD_MESSAGE);
        return;
    }
    user_info uInfo;
    strcpy(uInfo.UID, UID);
    strcpy(uInfo.password, password);

    int response = client_login(cInfo, uInfo);
    switch(response){
        case(LOGIN_ERROR):printf(LOGIN_ERROR_MESSAGE); return;
        case(LOGIN_SUCCESS):printf(LOGIN_SUCCESS_MESSAGE); break;
        case(LOGIN_NEW_USER):printf(LOGIN_NEW_USER_MESSAGE); break;
        case(LOGIN_WRONG_PASSWORD):printf(LOGIN_WRONG_PASSWORD_MESSAGE); return;
    }
    info = uInfo;
}

void logout(connection_info cInfo) {
    if(!isLoggedIn()){
        //This shouldn't happen but just in case
        printf(LOGOUT_NOT_SIGNED_IN_MESSAGE);
        return;
    }
    int response = client_logout(cInfo, info);
    switch(response){
        case(LOGOUT_ERROR):printf(LOGOUT_ERROR_MESSAGE); return;
        case(LOGOUT_SUCCESS):printf(LOGOUT_SUCCESS_MESSAGE); break;
        case(LOGOUT_NOT_SIGNED_IN):printf(LOGOUT_NOT_SIGNED_IN_MESSAGE); return;
        case(LOGOUT_NOT_REGISTERED):printf(LOGOUT_NOT_REGISTERED_MESSAGE); return;
        case(LOGOUT_WRONG_PASSWORD):printf(LOGOUT_WRONG_PASSWORD_MESSAGE); return;
    }

    info.UID[0] = '\0';
    info.password[0] = '\0';
}

void unregister(connection_info cInfo) {
    if(!isLoggedIn()){
        printf(UNREGISTER_NOT_SIGNED_IN_MESSAGE);
        return;
    }
    int response = client_unregister(cInfo, info);
    switch(response){
        case(UNREGISTER_ERROR):printf(UNREGISTER_ERROR_MESSAGE); return;
        case(UNREGISTER_SUCCESS):printf(UNREGISTER_SUCCESS_MESSAGE); break;
        case(UNREGISTER_NOT_SIGNED_IN):printf(UNREGISTER_NOT_SIGNED_IN_MESSAGE); return;
        case(UNREGISTER_NOT_REGISTERED):printf(UNREGISTER_NOT_REGISTERED_MESSAGE); return;
        case(UNREGISTER_WRONG_PASSWORD):printf(UNREGISTER_WRONG_PASSWORD_MESSAGE); return;
    }
    info.UID[0] = '\0';
    info.password[0] = '\0';
}

bool isLoggedIn(){
    return info.UID[0] != '\0' && info.password[0] != '\0';
}