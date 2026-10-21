#ifndef PROTOCOL_H
#define PROTOCOL_H

//Message tags and fixed values of the User-DS protocol, as they appear on the wire

//CLIENT REQUESTS
#define REQ_LOGIN "LIN"
#define REQ_LOGOUT "LOU"
#define REQ_UNREGISTER "UNR"
#define REQ_PUBLISH "PUB"
#define REQ_REMOVE_FILE "REM"
#define REQ_LIST_FILE "LST"
#define REQ_VERSIONS "VRS"

//SERVER ANSWERS
#define ANS_LOGIN "RLI"
#define ANS_LOGOUT "RLO"
#define ANS_UNREGISTER "RUR"
#define ANS_PUBLISH "RPB"
#define ANS_REMOVE_FILE "RRM"
#define ANS_LIST_FILE "RLS"
#define ANS_VERSIONS "RVR"

//Availability field of an RVR entry
#define AVAILABLE "AVL"
#define NOT_AVAILABLE "NAV"

//Longest valid RLS reply: "RLS NOK" or "RLS OK" plus up to 50 filenames of up
//to 24 characters, each preceded by a space, then "\n" and the terminator
#define MAX_LIST_FILE_REPLY_LENGTH (3 + 1 + 3 + 50 * (1 + 24) + 1 + 1)

#endif
