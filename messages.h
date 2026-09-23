#ifndef MESSAGES_H
#define MESSAGES_H

//GENERAL MESSAGES
#define NOT_SIGNED_IN_MESSAGE "User not logged in.\n"
#define NOT_REGISTERED_MESSAGE "Unknown user.\n"
#define WRONG_PASSWORD_MESSAGE "Incorrect password. You've been logged out.\n"
#define ALREADY_SIGNED_IN_MESSAGE "A user is logged in. Execute logout first.\n"
#define DS_UNEXPECTED_REPLY_MESSAGE "Unexpected reply received from the DS. Request not completed.\n"
#define DS_TIMEOUT_MESSAGE "No reply received from the DS. Request not completed.\n"

//LOGIN MESSAGES
#define LOGIN_SUCCESS_MESSAGE "Successful login.\n"
#define LOGIN_WRONG_PASSWORD_MESSAGE "Incorrect login attempt.\n"
#define LOGIN_NEW_USER_MESSAGE "New user registered.\n"

//LOGOUT MESSAGES
#define LOGOUT_SUCCESS_MESSAGE "Successful logout.\n"

//UNREGISTER MESSAGES
#define UNREGISTER_SUCCESS_MESSAGE "Successful unregister.\n"
#define UNREGISTER_NOT_LOGGED_OUT_MESSAGE "Must logout first.\n"

//PUBLISH MESSAGES
#define PUBLISH_SUCCESS_MESSAGE "Successful publication.\n"
#define PUBLISH_FAILED_MESSAGE "Unsuccessful publication.\n"

//REMOVE MESSAGES
#define REMOVE_FILE_SUCCESS_MESSAGE "Successful file removal.\n"
#define REMOVE_FILE_FAILED_MESSAGE "Resource not found.\n"

//SETUP (CLI ARGUMENT) MESSAGES
#define PEERPORT_MISSING_MESSAGE "Peerport is necessary.\n"
#define INVALID_PEERPORT_MESSAGE "Invalid peerport.\n"
#define INVALID_DSIP_MESSAGE "Invalid DS IP.\n"
#define INVALID_DSPORT_MESSAGE "Invalid DS port.\n"

//COMMAND MESSAGES
#define UNKNOWN_COMMAND_MESSAGE "Unknown command: %s\n"
#define INVALID_UID_MESSAGE "Invalid UID: must be exactly 6 digits.\n"
#define INVALID_PASSWORD_MESSAGE "Invalid password: must be exactly 8 alphanumeric characters.\n"
#define INVALID_FILENAME_MESSAGE "Invalid filename: max 24 characters, letters, digits, - and _, plus a dot and a 3 character extension.\n"
#define INVALID_LABEL_MESSAGE "Invalid label: must be 1 to 20 letters, digits, - or _.\n"
#define INVALID_FSIZE_MESSAGE "Invalid file size: must be at most 10000000 bytes.\n"
#define FILE_NOT_FOUND_MESSAGE "File not found in the local directory.\n"

#endif
