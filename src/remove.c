#include <stdio.h>
#include <string.h>
#include <sys/dirent.h>
#include <sys/stat.h>
#include <errno.h>

extern int errno;

struct stat sb;

int main(int argc, char *argv[]) {
    char *option = argv[2];
    for (int i = 3; i < 10; ++i) {
        char *fileName = argv[i];

        if(fileName == NULL ||  strcmp(fileName, "") == 0) break;

        else if(strncmp(option, "-i", 2) == 0) {

            // Check if filename is directory
            if (stat(fileName, &sb) == 0 && S_ISDIR(sb.st_mode)) {
                printf("    Can't remove %s, it is a directory\n", fileName);
            }
            else {
                char input;
                printf("    Remove file %s?\n", fileName);
                printf("    Enter y/n\n");
                scanf("    %c", &input);
                if(input == 'y') {
                    (remove(fileName) == 0) ?
                        printf("    %s Deleted successfully\n", fileName) :
                        printf("    %s\n", strerror(errno));
                }
                else if(input == 'n') {
                    printf("    %s not deleted\n", fileName);
                }
                else {
                    printf("    Invalid input\n");
                }
            }
        }

        else if(strncmp(option, " ", 1) == 0) {
            if (stat(fileName, &sb) == 0 && S_ISDIR(sb.st_mode)) {
                printf("    Can't remove %s, it is a directory\n", fileName);
            }
            else {
                (remove(fileName) == 0) ?
                    printf("    %s Deleted successfully\n", fileName) :
                    printf("    %s\n", strerror(errno));
            }
        }

        else if(strcmp(option, "-f") == 0) {
             (remove(fileName) == 0) ?
                printf("    %s Deleted successfully\n", fileName) :
                printf("    %s\n", strerror(errno));
        }

        else {
            printf("    Invalid option\n");
            break;
        }
    }
}
