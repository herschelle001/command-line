#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include "unistd.h"
#include <stdlib.h>
#include <errno.h>

extern int errno;

int main(int argc, char *argv[]) {
    char *option = argv[1];
    char *fileName = argv[2];

    time_t current_time;
    struct tm* ptime;
    time(&current_time);
    ptime = gmtime(&current_time);

    if(strcmp(option, "-u") == 0) {
        printf("    %s\n", asctime(ptime));
    }
    else if(strcmp(option, " ") == 0) {
        printf("    %s\n", ctime(&current_time));
    }
    else if(strcmp(option, "-r") == 0) {
        if(access(fileName, F_OK) == -1) {
            printf("    %s\n", strerror(errno));
            exit(errno);
        }
        struct stat attr;
        stat(fileName, &attr);
        printf("Last modified time: %s", ctime(&attr.st_mtime));
    }
    else {
        printf("    Invalid option\n");
    }
}
