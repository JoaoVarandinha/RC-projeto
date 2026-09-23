#include "commands.h"
#include "messages.h"
#include "validation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

user_info session;

void clear_session() {
    session.UID[0] = '\0';
    session.password[0] = '\0';
}

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

    else if (strcmp(cmdToken, "remove") == 0){
        char* filename = strtok(NULL, " \n");
        remove_file(cInfo, filename);
    }

    else if (strcmp(cmdToken, "versions") == 0){
        char* filename = strtok(NULL, " \n");
        versions(cInfo, filename);
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
        case(LOGIN_ERROR):printf(DS_UNEXPECTED_REPLY_MESSAGE); return;
        case(LOGIN_SUCCESS):printf(LOGIN_SUCCESS_MESSAGE); break;
        case(LOGIN_NEW_USER):printf(LOGIN_NEW_USER_MESSAGE); break;
        case(LOGIN_WRONG_PASSWORD):printf(LOGIN_WRONG_PASSWORD_MESSAGE); return;
        default: return; //api.c already reported the network failures
    }
    session = uInfo;
}

void logout(connection_info cInfo){
    if (!is_valid_logout(session)) return;

    int response = client_logout(cInfo, session);
    switch(response){
        case(LOGOUT_ERROR):printf(DS_UNEXPECTED_REPLY_MESSAGE); return;
        case(LOGOUT_SUCCESS):printf(LOGOUT_SUCCESS_MESSAGE); break;
        case(LOGOUT_NOT_SIGNED_IN):printf(NOT_SIGNED_IN_MESSAGE); break;
        case(LOGOUT_NOT_REGISTERED):printf(NOT_REGISTERED_MESSAGE); break;
        case(LOGOUT_WRONG_PASSWORD):printf(WRONG_PASSWORD_MESSAGE);break;
        default: return; //api.c already reported the network failures
    }

    clear_session();
}

void unregister(connection_info cInfo){
    if (!is_valid_unregister(session)) return;

    int response = client_unregister(cInfo, session);
    switch(response){
        case(UNREGISTER_ERROR):printf(DS_UNEXPECTED_REPLY_MESSAGE); return;
        case(UNREGISTER_SUCCESS):printf(UNREGISTER_SUCCESS_MESSAGE); break;
        case(UNREGISTER_NOT_SIGNED_IN):printf(NOT_SIGNED_IN_MESSAGE); break;
        case(UNREGISTER_NOT_REGISTERED):printf(NOT_REGISTERED_MESSAGE); break;
        case(UNREGISTER_WRONG_PASSWORD):printf(WRONG_PASSWORD_MESSAGE); break;
        default: return; //api.c already reported the network failures
    }
    clear_session();
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
        case(DS_TIMEOUT):printf(DS_TIMEOUT_MESSAGE); return;
        case(PUBLISH_ERROR):printf(DS_UNEXPECTED_REPLY_MESSAGE); return;
        case(PUBLISH_SUCCESS):printf(PUBLISH_SUCCESS_MESSAGE); return;
        case(PUBLISH_NOT_SIGNED_IN):printf(NOT_SIGNED_IN_MESSAGE); break;
        case(PUBLISH_NOT_REGISTERED):printf(NOT_REGISTERED_MESSAGE); break;
        case(PUBLISH_WRONG_PASSWORD):printf(WRONG_PASSWORD_MESSAGE); break;
        case(PUBLISH_FAILED):printf(PUBLISH_FAILED_MESSAGE); return;
        default: return; //api.c already reported the network failures
    }

    clear_session();
}

void remove_file(connection_info cInfo, char* filename){
    if (!is_valid_remove_file(session, filename)) return;

    int response = client_remove_file(cInfo, session, filename);
    switch(response){
        case(REMOVE_FILE_ERROR):printf(DS_UNEXPECTED_REPLY_MESSAGE); return;
        case(REMOVE_FILE_SUCCESS):printf(REMOVE_FILE_SUCCESS_MESSAGE); return;
        case(REMOVE_FILE_NOT_SIGNED_IN):printf(NOT_SIGNED_IN_MESSAGE); break;
        case(REMOVE_FILE_NOT_REGISTERED):printf(NOT_REGISTERED_MESSAGE); break;
        case(REMOVE_FILE_WRONG_PASSWORD):printf(WRONG_PASSWORD_MESSAGE); break;
        case(REMOVE_FILE_FAILED):printf(REMOVE_FILE_FAILED_MESSAGE); return;
        default: return; //api.c already reported the network failures
    }

    clear_session();
}


void versions(connection_info cInfo, char* filename){
    if (!is_valid_versions(filename)) return;

    char *reply = NULL;
    int response = client_versions(cInfo, filename, &reply);

    switch(response){
        case(VERSIONS_SUCCESS): break;
        case(VERSIONS_NO_PEERS): printf(VERSIONS_NO_PEERS_MESSAGE); return;
        case(VERSIONS_ERROR): printf(DS_UNEXPECTED_REPLY_MESSAGE); return;
        default: return; //api.c already reported the network failures
    }

    char *originalReply = reply;
    strtok(reply, " ");
    strtok(NULL, " ");
    while(true){
        char *UID = strtok(NULL, " \n");
        char *Fsize = strtok(NULL, " \n");
        char *label = strtok(NULL, " \n");
        char *pubTime = strtok(NULL, " \n");
        char *availability = strtok(NULL, " \n");
        if (!is_valid_version_reply(UID, Fsize, label, pubTime, availability)) break;
        
        if (!strcmp(availability,AVAILABLE)) availability = "Available";
        else availability = "Not Available";

        printf("\nUser: %s\nFile Size: %s\nMetaData: %s\nPublication Time: %s\nAvailability: %s\n", UID, Fsize, label, pubTime, availability);
    }
    free(originalReply);
    return;
}