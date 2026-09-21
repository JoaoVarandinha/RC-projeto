#include "validation.h"
#include "messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>

bool isLoggedIn(user_info uInfo){
    return uInfo.UID[0] != '\0' && uInfo.password[0] != '\0';
}

bool is_valid_PORT(char* PORT){
    if (PORT == NULL){
        return false;
    }
    //Check length (ports are 1 to 5 digits: 1-65535)
    size_t len = strlen(PORT);
    if (len < 1 || len > 5){
        return false;
    }
    //Check if all char are digits
    for (size_t i = 0; i < len; i++){
        if (!isdigit(PORT[i])){
            return false;
        }
    }
    //Check is number is an existing port
    int PORT_NUM = atoi(PORT);
    if (PORT_NUM < 1 || PORT_NUM > 65535){
        return false;
    }
    return true;
}

bool is_valid_UID(char* UID){
    if (UID == NULL){
        return false;
    }
    //Check length
    if (strlen(UID) != 6){
        return false;
    }
    //Check if all char are digits
    for (int i = 0; i < 6; i++){
        if (!isdigit(UID[i])){
            return false;
        }
    }
    return true;
}

bool is_valid_IP(char* IP){
    if (IP == NULL){
        return false;
    }
    struct in_addr addr;
    return inet_pton(AF_INET, IP, &addr) == 1;
}

bool is_valid_password(char* password){
    if (password == NULL){
        return false;
    }
    //Check length
    if (strlen(password) != 8){
        return false;
    }
    //Check if all char are letters or digits
    for (int i = 0; i < 8; i++){
        if (!isalnum(password[i])){
            return false;
        }
    }
    return true;
}

bool is_valid_filename(char* filename){
    if (filename == NULL) return false;

    size_t len = strlen(filename);
    if (len < 4 || len > 24) return false;

    //Check the dot is in the correct pos
    if (filename[len-4]!= '.') return false;

    //Check base
    for (int i = 0; i < len-4; i++){
        char c = filename[i];
        if (!isalnum(c) && !(c == '_' || c== '-'))
            return false;
    }

    //Check after the .
    for (int i = len-3; i < len; i++){
        if (!isalnum(filename[i])) return false;
    }
    return true;
}

bool is_valid_label(char* label){
    if (label == NULL) return false;

    size_t len = strlen(label);
    if (len < 4 || len > 20) return false;

    for (int i = 0; i < len; i++){
        char c = label[i];
        if (!isalnum(c) && !(c == '_' || c== '-'))
            return false;
    }
    return true;
}

bool is_valid_login(user_info uInfo, char* UID, char* password){
    if (isLoggedIn(uInfo)){
        //Prevents DS from keeping stale connection
        printf(ALREADY_SIGNED_IN_MESSAGE);
        return false;
    }
    if (!is_valid_UID(UID)){
        printf(INVALID_UID_MESSAGE);
        return false;
    }
    if (!is_valid_password(password)){
        printf(INVALID_PASSWORD_MESSAGE);
        return false;
    }
    return true;
}

bool is_valid_logout(user_info uInfo){
    if (!isLoggedIn(uInfo)){
        printf(NOT_SIGNED_IN_MESSAGE);
        return false;
    }
    return true;
}

bool is_valid_unregister(user_info uInfo){
    if (!isLoggedIn(uInfo)){
        printf(NOT_SIGNED_IN_MESSAGE);
        return false;
    }
    return true;
}

bool is_valid_publish(user_info uInfo, char* filename, char* label){
    if (!isLoggedIn(uInfo)){
        printf(NOT_SIGNED_IN_MESSAGE);
        return false;
    }
    if (!is_valid_filename(filename)){
        printf(INVALID_FILENAME_MESSAGE);
        return false;
    }
    if (!is_valid_label(label)){
        printf(INVALID_LABEL_MESSAGE);
        return false;
    }
    return true;
}
