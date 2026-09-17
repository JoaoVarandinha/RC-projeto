#include "commands.h"

void process_command(int fd, char* command) {
    char c[32];
    sscanf(command, "%s", &c);

    switch (c[0]) {
        case 'l':
            switch(c[3]) {
                case 'i':login(fd, command); break;
                case 'o':logout(fd); break;
                //case 't':break;
            }
            break;
        case 'u': unregister(fd); break;
        case 'e': exit(); break;
    }
}

void login(int fd, char* command) {

}

void logout(int fd) {

}

void unregister(int fd) {

}

void exit() {

}