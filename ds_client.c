#include "ds_client.h"
#include "net.h"
#include "protocol.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int client_login(connection_info cInfo, user_info uInfo){
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s %s\n", REQ_LOGIN, uInfo.UID, uInfo.password, cInfo.peerport);

    if (sendUDP(cInfo.sockfd, buf, strlen(buf)) == SEND_UDP_ERROR) return REQUEST_NOT_SENT;

    memset(buf, 0, sizeof(buf));

    int received = recvUDP(cInfo.sockfd, buf, sizeof(buf));
    if (received == RECV_TIMEOUT_ERROR) return DS_TIMEOUT;
    if (received == RECV_UDP_ERROR) return REPLY_NOT_RECEIVED;

    char* token = strtok(buf, " \n");
    if (token == NULL || strcmp(token, ANS_LOGIN)) return LOGIN_ERROR;

    char* status = strtok(NULL, " \n");
    if (status == NULL) return LOGIN_ERROR;

    if (strcmp(status, "OK") == 0) return LOGIN_SUCCESS;
    if (strcmp(status, "NOK") == 0) return LOGIN_WRONG_PASSWORD;
    if (strcmp(status, "REG") == 0) return LOGIN_NEW_USER;
    return LOGIN_ERROR;
}

int client_logout(connection_info cInfo, user_info uInfo){
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s\n", REQ_LOGOUT, uInfo.UID, uInfo.password);

    if (sendUDP(cInfo.sockfd, buf, strlen(buf)) == SEND_UDP_ERROR) return REQUEST_NOT_SENT;

    memset(buf, 0, sizeof(buf));

    int received = recvUDP(cInfo.sockfd, buf, sizeof(buf));
    if (received == RECV_TIMEOUT_ERROR) return DS_TIMEOUT;
    if (received == RECV_UDP_ERROR) return REPLY_NOT_RECEIVED;

    char* token = strtok(buf, " \n");
    if (token == NULL || strcmp(token, ANS_LOGOUT)) return LOGOUT_ERROR;

    char* status = strtok(NULL, " \n");
    if (status == NULL) return LOGOUT_ERROR;

    if (strcmp(status, "OK") == 0) return LOGOUT_SUCCESS;
    if (strcmp(status, "NLG") == 0) return LOGOUT_NOT_SIGNED_IN;
    if (strcmp(status, "UNR") == 0) return LOGOUT_NOT_REGISTERED;
    if (strcmp(status, "WRP") == 0) return LOGOUT_WRONG_PASSWORD;
    return LOGOUT_ERROR;
}

int client_unregister(connection_info cInfo, user_info uInfo){
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s\n", REQ_UNREGISTER, uInfo.UID, uInfo.password);

    if (sendUDP(cInfo.sockfd, buf, strlen(buf)) == SEND_UDP_ERROR) return REQUEST_NOT_SENT;

    memset(buf, 0, sizeof(buf));

    int received = recvUDP(cInfo.sockfd, buf, sizeof(buf));
    if (received == RECV_TIMEOUT_ERROR) return DS_TIMEOUT;
    if (received == RECV_UDP_ERROR) return REPLY_NOT_RECEIVED;

    char* token = strtok(buf, " \n");
    if (token == NULL || strcmp(token, ANS_UNREGISTER)) return UNREGISTER_ERROR;

    char* status = strtok(NULL, " \n");
    if (status == NULL) return UNREGISTER_ERROR;

    if (strcmp(status, "OK") == 0) return UNREGISTER_SUCCESS;
    if (strcmp(status, "NOK") == 0) return UNREGISTER_NOT_SIGNED_IN;
    if (strcmp(status, "UNR") == 0) return UNREGISTER_NOT_REGISTERED;
    if (strcmp(status, "WRP") == 0) return UNREGISTER_WRONG_PASSWORD;
    return UNREGISTER_ERROR;
}

int client_publish(connection_info cInfo, user_info uInfo, file_info fInfo){
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s %s %zu %s\n", REQ_PUBLISH, uInfo.UID, uInfo.password, fInfo.filename, fInfo.filesize, fInfo.label);

    if (sendUDP(cInfo.sockfd, buf, strlen(buf)) == SEND_UDP_ERROR) return REQUEST_NOT_SENT;

    memset(buf, 0, sizeof(buf));

    int received = recvUDP(cInfo.sockfd, buf, sizeof(buf));
    if (received == RECV_TIMEOUT_ERROR) return DS_TIMEOUT;
    if (received == RECV_UDP_ERROR) return REPLY_NOT_RECEIVED;

    char* token = strtok(buf, " \n");
    if (token == NULL || strcmp(token, ANS_PUBLISH)) return PUBLISH_ERROR;

    char* status = strtok(NULL, " \n");
    if (status == NULL) return PUBLISH_ERROR;

    if (strcmp(status, "OK") == 0) return PUBLISH_SUCCESS;
    if (strcmp(status, "NLG") == 0) return PUBLISH_NOT_SIGNED_IN;
    if (strcmp(status, "UNR") == 0) return PUBLISH_NOT_REGISTERED;
    if (strcmp(status, "WRP") == 0) return PUBLISH_WRONG_PASSWORD;
    if (strcmp(status, "NOK") == 0) return PUBLISH_FAILED;
    return PUBLISH_ERROR;
}

int client_remove_file(connection_info cInfo, user_info uInfo, char* filename){
    char buf[MAX_INSTRUCTION_LENGTH];
    snprintf(buf, sizeof(buf), "%s %s %s %s\n", REQ_REMOVE_FILE, uInfo.UID, uInfo.password, filename);

    if (sendUDP(cInfo.sockfd, buf, strlen(buf)) == SEND_UDP_ERROR) return REQUEST_NOT_SENT;

    memset(buf, 0, sizeof(buf));

    int received = recvUDP(cInfo.sockfd, buf, sizeof(buf));
    if (received == RECV_TIMEOUT_ERROR) return DS_TIMEOUT;
    if (received == RECV_UDP_ERROR) return REPLY_NOT_RECEIVED;

    char* token = strtok(buf, " \n");
    if (token == NULL || strcmp(token, ANS_REMOVE_FILE)) return REMOVE_FILE_ERROR;

    char* status = strtok(NULL, " \n");
    if (status == NULL) return REMOVE_FILE_ERROR;

    if (strcmp(status, "OK") == 0) return REMOVE_FILE_SUCCESS;
    if (strcmp(status, "NLG") == 0) return REMOVE_FILE_NOT_SIGNED_IN;
    if (strcmp(status, "UNR") == 0) return REMOVE_FILE_NOT_REGISTERED;
    if (strcmp(status, "WRP") == 0) return REMOVE_FILE_WRONG_PASSWORD;
    if (strcmp(status, "NOK") == 0) return REMOVE_FILE_FAILED;
    return REMOVE_FILE_ERROR;
}

//sscanf reads without writing into reply, unlike strtok, so the file list stays
//intact for the caller
static int list_file_status(const char* reply){
    char prefix[8], status[8];
    if (sscanf(reply, "%7s %7s", prefix, status) != 2) return LIST_FILE_ERROR;
    if (strcmp(prefix, ANS_LIST_FILE)) return LIST_FILE_ERROR;

    if (strcmp(status, "OK") == 0) return LIST_FILE_SUCCESS;
    if (strcmp(status, "NOK") == 0) return LIST_FILE_NO_RESOURCES;
    return LIST_FILE_ERROR;
}

int client_list_file(connection_info cInfo, char** reply){
    *reply = NULL;

    char request[MAX_INSTRUCTION_LENGTH];
    snprintf(request, sizeof(request), "%s\n", REQ_LIST_FILE);

    //Allocated before sending, so a failure here cannot leave an unread reply
    //waiting in the socket for the next command to pick up
    char* buf = malloc(MAX_LIST_FILE_REPLY_LENGTH);
    if (buf == NULL) return LIST_FILE_ERROR;

    if (sendUDP(cInfo.sockfd, request, strlen(request)) == SEND_UDP_ERROR){
        free(buf);
        return REQUEST_NOT_SENT;
    }

    memset(buf, 0, MAX_LIST_FILE_REPLY_LENGTH);

    int received = recvUDP(cInfo.sockfd, buf, MAX_LIST_FILE_REPLY_LENGTH);
    if (received == RECV_TIMEOUT_ERROR){free(buf); return DS_TIMEOUT;}
    if (received == RECV_UDP_ERROR){free(buf); return REPLY_NOT_RECEIVED;}

    //A valid reply never reaches the last byte, but a malformed one that fills
    //the buffer would otherwise be left without a terminator
    buf[MAX_LIST_FILE_REPLY_LENGTH - 1] = '\0';

    int status = list_file_status(buf);

    if (status == LIST_FILE_SUCCESS) *reply = buf;
    else free(buf);

    return status;
}

//sscanf reads without writing into reply, unlike strtok, so the peer list stays
//intact for the caller
static int versions_status(const char* reply){
    char prefix[8], status[8];
    if (sscanf(reply, "%7s %7s", prefix, status) != 2) return VERSIONS_ERROR;
    if (strcmp(prefix, ANS_VERSIONS)) return VERSIONS_ERROR;

    if (strcmp(status, "OK") == 0) return VERSIONS_SUCCESS;
    if (strcmp(status, "NOK") == 0) return VERSIONS_NO_PEERS;
    return VERSIONS_ERROR;
}

int client_versions(connection_info cInfo, char* filename, char** reply){
    *reply = NULL;

    int fd = connectTCP(cInfo);
    if (fd == TCP_CONNECT_ERROR) return REQUEST_NOT_SENT;

    char request[MAX_INSTRUCTION_LENGTH];
    snprintf(request, sizeof(request), "%s %s\n", REQ_VERSIONS, filename);

    //Single exit from here on, so the socket and the buffer are released once
    int status;
    if (sendTCP(fd, request, strlen(request)) == SEND_TCP_ERROR){
        status = REQUEST_NOT_SENT;
    }
    else {
        char* received = NULL;
        int response = recvTCP(fd, &received);

        if (response == RECV_TIMEOUT_ERROR) status = DS_TIMEOUT;
        else if (response == RECV_TCP_ERROR) status = REPLY_NOT_RECEIVED;
        else {
            status = versions_status(received);
            if (status == VERSIONS_SUCCESS) *reply = received;
            else free(received);
        }
    }

    disconnectTCP(fd);
    return status;
}
