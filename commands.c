#include "commands.h"
#include "messages.h"
#include "validation.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

user_info session;

int process_command(connection_info cInfo, char* command){
    char *cmdToken = strtok(command, " \n");
    if (cmdToken == NULL) return 0;

    if (strcmp(cmdToken, "login") == 0){
        char* UID = strtok(NULL, " \n");
        char* password = strtok(NULL, " \n");
        login(cInfo, UID, password);
    }

    else if (strcmp(cmdToken, "logout") == 0){
        logout(cInfo);
    }

    else if (strcmp(cmdToken, "unregister") == 0){
        unregister(cInfo);
    }

    else if (strcmp(cmdToken, "publish") == 0){
        char* filename = strtok(NULL, " \n");
        char* label = strtok(NULL, " \n");
        publish(cInfo, filename, label);
    }

    else if (strcmp(cmdToken, "exit") == 0){
        if (isLoggedIn(session)) printf(ALREADY_SIGNED_IN_MESSAGE);
        else return EXIT_CODE;
    }

    else {
        printf(UNKNOWN_COMMAND_MESSAGE, command);
    }
    return 0;
}

void login(connection_info cInfo, char* UID, char* password){
    if (!is_valid_login(session, UID, password)) return;

    user_info uInfo;
    strcpy(uInfo.UID, UID);
    strcpy(uInfo.password, password);

    int response = client_login(cInfo, uInfo);
    switch(response){
        case(DS_TIMEOUT):printf(DS_TIMEOUT_MESSAGE); return;
        case(LOGIN_ERROR):printf(DS_UNEXPECTED_REPLY_MESSAGE); return;
        case(LOGIN_SUCCESS):printf(LOGIN_SUCCESS_MESSAGE); break;
        case(LOGIN_NEW_USER):printf(LOGIN_NEW_USER_MESSAGE); break;
        case(LOGIN_WRONG_PASSWORD):printf(LOGIN_WRONG_PASSWORD_MESSAGE); return;
    }
    session = uInfo;
}

void logout(connection_info cInfo){
    if (!is_valid_logout(session)) return;

    int response = client_logout(cInfo, session);
    switch(response){
        case(DS_TIMEOUT):printf(DS_TIMEOUT_MESSAGE); return;
        case(LOGOUT_ERROR):printf(DS_UNEXPECTED_REPLY_MESSAGE); return;
        case(LOGOUT_SUCCESS):printf(LOGOUT_SUCCESS_MESSAGE); break;
        case(LOGOUT_NOT_SIGNED_IN):printf(NOT_SIGNED_IN_MESSAGE); return;
        case(LOGOUT_NOT_REGISTERED):printf(LOGOUT_NOT_REGISTERED_MESSAGE); return;
        case(LOGOUT_WRONG_PASSWORD):printf(LOGOUT_WRONG_PASSWORD_MESSAGE); return;
    }

    session.UID[0] = '\0';
    session.password[0] = '\0';
}

void unregister(connection_info cInfo){
    if (!is_valid_unregister(session)) return;

    int response = client_unregister(cInfo, session);
    switch(response){
        case(DS_TIMEOUT):printf(DS_TIMEOUT_MESSAGE); return;
        case(UNREGISTER_ERROR):printf(DS_UNEXPECTED_REPLY_MESSAGE); return;
        case(UNREGISTER_SUCCESS):printf(UNREGISTER_SUCCESS_MESSAGE); break;
        case(UNREGISTER_NOT_SIGNED_IN):printf(NOT_SIGNED_IN_MESSAGE); return;
        case(UNREGISTER_NOT_REGISTERED):printf(UNREGISTER_NOT_REGISTERED_MESSAGE); return;
        case(UNREGISTER_WRONG_PASSWORD):printf(UNREGISTER_WRONG_PASSWORD_MESSAGE); return;
    }
    session.UID[0] = '\0';
    session.password[0] = '\0';
}

void publish(connection_info cInfo, char* filename, char* label){
    if (!is_valid_publish(session, filename, label)) return;

    struct stat st;
    if (stat(filename, &st) != 0){
        printf(FILE_NOT_FOUND_MESSAGE);
        return;
    }

    if (st.st_size > MAX_FSIZE){
        printf(INVALID_FSIZE_MESSAGE);
        return;
    }

    file_info fInfo;
    fInfo.filesize = st.st_size;
    strcpy(fInfo.filename, filename);
    strcpy(fInfo.label, label);

    int response = client_publish(cInfo, session, fInfo);
    switch(response){
        case(DS_TIMEOUT):printf(DS_TIMEOUT_MESSAGE); break;
        case(PUBLISH_ERROR):printf(DS_UNEXPECTED_REPLY_MESSAGE); break;
        case(PUBLISH_SUCCESS):printf(PUBLISH_SUCCESS_MESSAGE); break;
        case(PUBLISH_NOT_SIGNED_IN):printf(NOT_SIGNED_IN_MESSAGE); break;
        case(PUBLISH_NOT_REGISTERED):printf(PUBLISH_NOT_REGISTERED_MESSAGE); break;
        case(PUBLISH_WRONG_PASSWORD):printf(PUBLISH_WRONG_PASSWORD_MESSAGE); break;
        case(PUBLISH_FAILED):printf(PUBLISH_FAILED_MESSAGE); break;
    }
}