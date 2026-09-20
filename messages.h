#ifndef MESSAGES_H
#define MESSAGES_H

//LOGIN MESSAGES
#define LOGIN_ERROR_MESSAGE "Error: invalid reply received from the DS.\n"
#define LOGIN_SUCCESS_MESSAGE "Successful login.\n"
#define LOGIN_WRONG_PASSWORD_MESSAGE "Incorrect login attempt.\n"
#define LOGIN_NEW_USER_MESSAGE "New user registered.\n"

//LOGOUT MESSAGES
#define LOGOUT_ERROR_MESSAGE "Error: invalid reply received from the DS.\n"
#define LOGOUT_SUCCESS_MESSAGE "Successful logout.\n"
#define LOGOUT_NOT_SIGNED_IN_MESSAGE "User not logged in.\n"
#define LOGOUT_NOT_REGISTERED_MESSAGE "Unknown user.\n"
#define LOGOUT_WRONG_PASSWORD_MESSAGE "Incorrect password.\n"

//UNREGISTER MESSAGES
#define UNREGISTER_ERROR_MESSAGE "Error: invalid reply received from the DS.\n"
#define UNREGISTER_SUCCESS_MESSAGE "Successful unregister.\n"
#define UNREGISTER_NOT_SIGNED_IN_MESSAGE "User not logged in.\n"
#define UNREGISTER_NOT_REGISTERED_MESSAGE "Unknown user.\n"
#define UNREGISTER_WRONG_PASSWORD_MESSAGE "Incorrect password.\n"

//SETUP (CLI ARGUMENT) MESSAGES
#define PEERPORT_MISSING_MESSAGE "Peerport is necessary.\n"
#define INVALID_PEERPORT_MESSAGE "Invalid peerport.\n"
#define INVALID_DSIP_MESSAGE "Invalid DS IP.\n"
#define INVALID_DSPORT_MESSAGE "Invalid DS port.\n"

//COMMAND MESSAGES
#define UNKNOWN_COMMAND_MESSAGE "Unknown command: %s\n"
#define INVALID_UID_MESSAGE "Invalid UID: must be exactly 6 digits.\n"
#define INVALID_PASSWORD_MESSAGE "Invalid password: must be exactly 8 alphanumeric characters.\n"

#endif
