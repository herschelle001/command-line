#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

extern int errno;

int main(int argc, char *argv[]) {
    char pathName[100];
    char *option = argv[2];
    for (int i = 3; i < 10; ++i) {
        char *fileName = argv[i];
        struct stat st = {0};

        if(strncmp(option, " ", 1) == 0) {
            getcwd(pathName, 100);
            strcat(pathName, "/");
            strncat(pathName, fileName, strlen(fileName));

            if (stat(pathName, &st) == -1) {
                if(mkdir(pathName, 0700) != 0) {
                    printf("    %s\n", strerror(errno));
                }
            }
            else {
                printf("    Directory already exists\n");
            }
        }
        else if(strcmp(option, "-v") == 0) {
            getcwd(pathName, 100);
            strcat(pathName, "/");
            strncat(pathName, fileName, strlen(fileName));

            if (stat(pathName, &st) == -1) {
                (mkdir(pathName, 0700) == 0) ?
                printf("    mkdir : created directory '%s'\n", fileName) :
                printf("    %s\n", strerror(errno));
            }
            else {
                printf("    Directory already exists");
            }
        }
        else if(strcmp(option, "-p") == 0) {
            getcwd(pathName, 100);
            strcat(pathName, "/");
            char *token = strtok(fileName, "/");
            while (token != NULL) {
                strcat(pathName, token);

                if (stat(pathName, &st) == -1) {
                    (mkdir(pathName, 0700) == 0) ?
                    printf("    mkdir : created directory '%s'\n", token) :
                    printf("    Failed to make directory\n");
                }
                else {
                    printf("    Directory already exists");
                }

                strcat(pathName, "/");
                if(chdir(token) == -1) {
                    printf("    %s\n", strerror(errno));
                    exit(errno);
                }

                token = strtok(NULL, "/");
                if(token[strlen(token)-1] == '\n') {
                    token = strtok(token, "\n");
                }
            }
        }
        else {
            printf("    Invalid option\n");
            break;
        }

        char *empty = "";
        if(fileName == NULL || strcmp(fileName, empty) == 0) break;
    }
}