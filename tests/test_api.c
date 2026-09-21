// Tests for the client_* protocol functions in api.c.
//
// Each case binds a real UDP socket on loopback to act as the DS, points a
// connection_info at it, and calls the real client_* function. That exercises
// message construction, sendto, recvfrom and reply parsing end to end.
//
// The mock DS runs in a forked child: the socket is bound before the fork, so a
// request sent by the parent is queued and waiting whenever the child gets
// scheduled. No sleeps or retries needed.
//
// Build and run from the project root:
//   gcc -Wall -o tests/test_api tests/test_api.c api.c && ./tests/test_api

#include "../api.h"
#include "../parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TEST_UID "123456"
#define TEST_PASSWORD "abcdefgh"
#define TEST_PEERPORT "58000"
#define TEST_FILENAME "video.mp4"
#define TEST_LABEL "1080p"
#define TEST_FSIZE 1048576

typedef enum {
    CMD_LOGIN,
    CMD_LOGOUT,
    CMD_UNREGISTER,
    CMD_PUBLISH
} command_kind;

static int checks = 0;
static int failures = 0;

static void check_int(const char* what, int got, int want){
    checks++;
    if (got != want){
        failures++;
        printf("  FAIL  %s: returned %d, expected %d\n", what, got, want);
    }
}

static void check_str(const char* what, const char* got, const char* want){
    checks++;
    if (strcmp(got, want) != 0){
        failures++;
        printf("  FAIL  %s:\n          sent     \"%s\"\n          expected \"%s\"\n", what, got, want);
    }
}

//Binds a UDP socket to an ephemeral loopback port and writes that port number
//into portOut, so the client can be aimed at it.
static int bind_mock_ds(char* portOut, size_t portOutLen){
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1){
        printf("could not create mock DS socket\n");
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0; //let the kernel pick a free port

    if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) == -1){
        printf("could not bind mock DS socket\n");
        exit(1);
    }

    socklen_t addrlen = sizeof(addr);
    getsockname(sockfd, (struct sockaddr*) &addr, &addrlen);
    snprintf(portOut, portOutLen, "%d", ntohs(addr.sin_port));
    return sockfd;
}

//Calls the client_* function under test with fixed credentials.
static int call_command(command_kind kind, connection_info cInfo){
    user_info uInfo;
    strcpy(uInfo.UID, TEST_UID);
    strcpy(uInfo.password, TEST_PASSWORD);

    file_info fInfo;
    strcpy(fInfo.filename, TEST_FILENAME);
    strcpy(fInfo.label, TEST_LABEL);
    fInfo.filesize = TEST_FSIZE;

    switch(kind){
        case CMD_LOGIN: return client_login(cInfo, uInfo);
        case CMD_LOGOUT: return client_logout(cInfo, uInfo);
        case CMD_UNREGISTER: return client_unregister(cInfo, uInfo);
        case CMD_PUBLISH: return client_publish(cInfo, uInfo, fInfo);
    }
    return 0;
}

//Runs one request/reply exchange against the mock DS.
//reply may be NULL, which makes the DS stay silent so the client times out.
//requestOut, when given, receives the exact bytes the client sent.
static int exchange(command_kind kind, const char* reply, char* requestOut, size_t requestOutLen){
    char port[8];
    int dsSocket = bind_mock_ds(port, sizeof(port));

    int requestPipe[2];
    if (pipe(requestPipe) == -1){
        printf("could not create pipe\n");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1){
        printf("could not fork mock DS\n");
        exit(1);
    }

    if (pid == 0){
        //Mock DS: take one request, report it back, answer it
        close(requestPipe[0]);

        char request[MAX_INSTRUCTION_LENGTH];
        struct sockaddr_in from;
        socklen_t fromlen = sizeof(from);
        ssize_t n = recvfrom(dsSocket, request, sizeof(request) - 1, 0,
                             (struct sockaddr*) &from, &fromlen);
        if (n < 0) _exit(1);
        request[n] = '\0';

        if (write(requestPipe[1], request, n) != n) _exit(1);
        close(requestPipe[1]);

        if (reply != NULL){
            sendto(dsSocket, reply, strlen(reply), 0, (struct sockaddr*) &from, fromlen);
        }
        _exit(0);
    }

    //Parent: act as the User application
    close(requestPipe[1]);

    connection_info cInfo;
    cInfo.peerport = TEST_PEERPORT;
    cInfo.dsip = "127.0.0.1";
    cInfo.dsport = port;
    connectUDP(&cInfo);

    int response = call_command(kind, cInfo);

    if (requestOut != NULL){
        ssize_t n = read(requestPipe[0], requestOut, requestOutLen - 1);
        requestOut[n < 0 ? 0 : n] = '\0';
    }

    close(requestPipe[0]);
    waitpid(pid, NULL, 0);
    disconnectUDP();
    close(dsSocket);
    return response;
}

//Checks the exact bytes each command puts on the wire against the spec
static void test_request_format(){
    printf("request format\n");
    char request[MAX_INSTRUCTION_LENGTH];

    exchange(CMD_LOGIN, "RLI OK\n", request, sizeof(request));
    check_str("LIN", request, "LIN " TEST_UID " " TEST_PASSWORD " " TEST_PEERPORT "\n");

    exchange(CMD_LOGOUT, "RLO OK\n", request, sizeof(request));
    check_str("LOU", request, "LOU " TEST_UID " " TEST_PASSWORD "\n");

    exchange(CMD_UNREGISTER, "RUR OK\n", request, sizeof(request));
    check_str("UNR", request, "UNR " TEST_UID " " TEST_PASSWORD "\n");

    exchange(CMD_PUBLISH, "RPB OK\n", request, sizeof(request));
    check_str("PUB", request, "PUB " TEST_UID " " TEST_PASSWORD " "
                              TEST_FILENAME " 1048576 " TEST_LABEL "\n");
}

static void test_login_replies(){
    printf("client_login replies\n");
    check_int("RLI OK", exchange(CMD_LOGIN, "RLI OK\n", NULL, 0), LOGIN_SUCCESS);
    check_int("RLI NOK", exchange(CMD_LOGIN, "RLI NOK\n", NULL, 0), LOGIN_WRONG_PASSWORD);
    check_int("RLI REG", exchange(CMD_LOGIN, "RLI REG\n", NULL, 0), LOGIN_NEW_USER);
    check_int("RLI ERR", exchange(CMD_LOGIN, "RLI ERR\n", NULL, 0), LOGIN_ERROR);
}

static void test_logout_replies(){
    printf("client_logout replies\n");
    check_int("RLO OK", exchange(CMD_LOGOUT, "RLO OK\n", NULL, 0), LOGOUT_SUCCESS);
    check_int("RLO NLG", exchange(CMD_LOGOUT, "RLO NLG\n", NULL, 0), LOGOUT_NOT_SIGNED_IN);
    check_int("RLO UNR", exchange(CMD_LOGOUT, "RLO UNR\n", NULL, 0), LOGOUT_NOT_REGISTERED);
    check_int("RLO WRP", exchange(CMD_LOGOUT, "RLO WRP\n", NULL, 0), LOGOUT_WRONG_PASSWORD);
    check_int("RLO ERR", exchange(CMD_LOGOUT, "RLO ERR\n", NULL, 0), LOGOUT_ERROR);
}

static void test_unregister_replies(){
    printf("client_unregister replies\n");
    check_int("RUR OK", exchange(CMD_UNREGISTER, "RUR OK\n", NULL, 0), UNREGISTER_SUCCESS);
    check_int("RUR NOK", exchange(CMD_UNREGISTER, "RUR NOK\n", NULL, 0), UNREGISTER_NOT_SIGNED_IN);
    check_int("RUR UNR", exchange(CMD_UNREGISTER, "RUR UNR\n", NULL, 0), UNREGISTER_NOT_REGISTERED);
    check_int("RUR WRP", exchange(CMD_UNREGISTER, "RUR WRP\n", NULL, 0), UNREGISTER_WRONG_PASSWORD);
    check_int("RUR ERR", exchange(CMD_UNREGISTER, "RUR ERR\n", NULL, 0), UNREGISTER_ERROR);
}

static void test_publish_replies(){
    printf("client_publish replies\n");
    check_int("RPB OK", exchange(CMD_PUBLISH, "RPB OK\n", NULL, 0), PUBLISH_SUCCESS);
    check_int("RPB UNR", exchange(CMD_PUBLISH, "RPB UNR\n", NULL, 0), PUBLISH_NOT_REGISTERED);
    check_int("RPB WRP", exchange(CMD_PUBLISH, "RPB WRP\n", NULL, 0), PUBLISH_WRONG_PASSWORD);
    check_int("RPB ERR", exchange(CMD_PUBLISH, "RPB ERR\n", NULL, 0), PUBLISH_ERROR);
    //NLG and NOK share a first letter, so these two must not collapse into one
    check_int("RPB NLG", exchange(CMD_PUBLISH, "RPB NLG\n", NULL, 0), PUBLISH_NOT_SIGNED_IN);
    check_int("RPB NOK", exchange(CMD_PUBLISH, "RPB NOK\n", NULL, 0), PUBLISH_FAILED);
}

//The spec requires a malformed reply to stop the interaction and inform the user
static void test_malformed_replies(){
    printf("malformed replies\n");
    check_int("wrong prefix", exchange(CMD_LOGIN, "RLO OK\n", NULL, 0), LOGIN_ERROR);
    check_int("missing status", exchange(CMD_LOGIN, "RLI\n", NULL, 0), LOGIN_ERROR);
    check_int("unknown status", exchange(CMD_LOGIN, "RLI XYZ\n", NULL, 0), LOGIN_ERROR);
    check_int("status prefix only", exchange(CMD_LOGIN, "RLI O\n", NULL, 0), LOGIN_ERROR);
    check_int("empty reply", exchange(CMD_LOGIN, "\n", NULL, 0), LOGIN_ERROR);
    check_int("junk", exchange(CMD_LOGIN, "hello there\n", NULL, 0), LOGIN_ERROR);
    //A longer status starting with a valid one must not be accepted
    check_int("OKAY not OK", exchange(CMD_LOGIN, "RLI OKAY\n", NULL, 0), LOGIN_ERROR);
    check_int("NOKIA not NOK", exchange(CMD_PUBLISH, "RPB NOKIA\n", NULL, 0), PUBLISH_ERROR);
}

//Silence from the DS must be reported as a timeout, not as a bad reply
static void test_timeout(){
    printf("timeout (waits %d seconds)\n", RECV_TIMEOUT);
    check_int("no reply", exchange(CMD_LOGIN, NULL, NULL, 0), DS_TIMEOUT);
}

int main(){
    test_request_format();
    test_login_replies();
    test_logout_replies();
    test_unregister_replies();
    test_publish_replies();
    test_malformed_replies();
    test_timeout();

    printf("\n%d checks, %d failures\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
