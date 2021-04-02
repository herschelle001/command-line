#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/dir.h>
#include <stdlib.h>
#include <pwd.h>
#include <errno.h>
#include <sys/stat.h>

extern int errno;

#define HISTORY_MAX_SIZE 10

char *historyArray[HISTORY_MAX_SIZE];
int historyCount = 0;

int idx = 5;
char *pathTokens[100] = {""};

char initialDirectory[100];
char currentDirectory[100];

struct direct **files;
struct passwd *pw;
struct stat sb;

void addCommandToHistory(char *);
void fillArray(char **);
void changeDirectory(void);
void printWorkingDirectory(void);
void listItems(void);
void concatenate(void);
void date(void);
void removeFile(void);
void makeDirectory(void);
void printLogicalPath(void);
char* calculateLogicalPath(void);

int main() {
    getcwd(initialDirectory, 100);

    // get user's home directory
    pw = getpwuid(getuid());

    if (pw == NULL) {
        printf("    %s\n", strerror(errno));
        exit(errno);
    }

    // set current directory to user's home directory
    chdir(pw->pw_dir);
    for (int i = 0; i < 100; i+=2) {
        pathTokens[i] = "/";
    }
    pathTokens[1] = strtok(pw->pw_dir, "/");
    pathTokens[3] = strtok(NULL, "/");

    char input[100];

    while (1) {
        printf("Enter command: ");
        fgets(input, 100, stdin);
        if(strncmp(input, "history", 7) != 0)
            addCommandToHistory(input);
        char *command = strtok(input, " ");

        // exit
        if(strncmp(command, "exit", 4) == 0) {
            exit(0);
        }

        // pwd -L -P
        else if(strncmp(command, "pwd", 3) == 0) {
            printWorkingDirectory();
        }

        // echo
        else if(strncmp(command, "echo", 4) == 0) {
            char *message = strtok(NULL, " ");
            while (message) {
                printf("%s ", message);
                message = strtok(NULL, " ");
            }
            printf("\n");
        }

        // history
        else if(strncmp(command, "history", 7) == 0) {
            for (int i = 0; i < historyCount; ++i) {
                printf("    %d %s", i+1, historyArray[i]);
            }
        }

        // cd -L -P
        else if(strncmp(command, "cd", 2) == 0) {
            changeDirectory();
        }

        // ls -a -1
        else if(strncmp(command, "ls", 2) == 0) {
            listItems();
        }

        // cat > -E -n
        else if(strncmp(command, "cat", 3) == 0) {
            concatenate();
        }

        // date -r -u
        else if(strncmp(command, "date", 4) == 0) {
            date();
        }

        // rm -i -f
        else if(strncmp(command, "rm", 2) == 0) {
            removeFile();
        }

        // mkdir -v -p
        else if(strncmp(command, "mkdir", 5) == 0) {
            makeDirectory();
        }
        else {
            printf("    Invalid command\n");
        }
    }
    return 0;
}

void changeDirectory() {
    char *option = strtok(NULL, " "),
         *fileName = strtok(NULL, " ");

    if(strcmp(option, "~") == 0) {
        chdir(pw->pw_dir);
        idx = 5;
        printLogicalPath();
        return;
    }

    if(fileName == NULL) {
        fileName = option;
        option = " ";
    }

    if(strcmp(option, " ") == 0 || strcmp(option, "-P") == 0 || strcmp(option, "-L") == 0) {
        int l = strlen(fileName);
        size_t size = l + 1;
        char *c = malloc(size);

        if (fileName[l - 1] == '\\') {
            strncpy(c, fileName, l - 1);
            do {
                char *token = strtok(NULL, " ");
                strcat(c, " ");
                strcat(c, token);
                l = strlen(c);
            } while (c[l - 1] == '\\');
        }
        else {
            strcpy(c, fileName);
        }

        c = strtok(c, "\n");
        if(chdir(c) != 0) {
            printf("%s\n", strerror(errno));
            return;
        }
        getcwd(currentDirectory, 100);

        char *currentLogicalPath = calculateLogicalPath();

        if(strcmp(option, "-L") == 0 && strncmp(fileName, "..", 2) == 0) {
            idx -= 2;
            currentLogicalPath = calculateLogicalPath();
            printf("    %s\n", currentLogicalPath);
            chdir(currentLogicalPath);
        }

        else if( (strcmp(option, "-P") == 0 && strncmp(fileName, "..", 2) == 0) || (strcmp(option, "-P") == 0 && strlen(currentLogicalPath) >= strlen(currentDirectory)) ) {
            currentLogicalPath = calculateLogicalPath();
            while (strcmp(currentLogicalPath, currentDirectory) != 0) {
                idx -= 2;
                currentLogicalPath = calculateLogicalPath();
            }
            printf("    %s\n", getcwd(currentDirectory, 100));
        }

        else {
            char *cToken = strtok(c, "/");
            while (cToken) {
                pathTokens[idx] = cToken;
                idx += 2;
                cToken = strtok(NULL, "/");
            }
            strcmp(option, "-P") != 0 ? printLogicalPath() :
                printf("    %s\n", getcwd(currentDirectory, 100));
        }
    }

    else {
        printf("    Invalid option\n");
    }
}

// External Commands

void printWorkingDirectory() {
    char *option = strtok(NULL, " ");
    option = strtok(option, "\n");

    if(option == NULL || strcmp(option, "-L") == 0) {
        printLogicalPath();
    }
    else if(strcmp(option, "-P") == 0){
        printf("    %s\n", getcwd(currentDirectory, 100));
    }
    else
        printf("    Invalid option\n");
}

void listItems() {
    char *option = strtok(NULL, " ");
    option = strtok(option, "\n");

    if(option == NULL) {
        option = " ";
    }

    int f = fork();
    if(f < 0) {
        printf("    %s\n", strerror(errno));
        return;
    }
    if(f == 0) {
        getcwd(currentDirectory, 100);
        strcat(initialDirectory, "/list");

        execl(initialDirectory, "list", option, files, NULL);
        printf("    %s\n", strerror(errno));
    }
    else {
        wait(NULL);
//                printf("    ls process complete\n");
    }
}

void concatenate() {
    char *token = strtok(NULL, " "),
         *option = strtok(NULL, " "),
         *fileName = strtok(NULL, " ");

    token = strtok(token, "\n");
    option = strtok(option, "\n");

    fileName = strtok(fileName, "\n");
    if(option == NULL) {
        option = " ";
    }

    int f = fork();
    if (f < 0) {
        printf("    %s\n", strerror(errno));
        exit(errno);
    }
    if (f == 0) {
        strcat(initialDirectory, "/cat");

        execl(initialDirectory, "cat", token, option, fileName, NULL);
        printf("    %s\n", strerror(errno));
    }
    else {
        wait(NULL);
//                printf("    cat process complete\n");
    }
}

void date() {
    char *option = strtok(NULL, " "),
         *fileName = strtok(NULL, " ");

    option = strtok(option, "\n");
    fileName = strtok(fileName, "\n");

    if(option == NULL) {
        option = " ";
    }

    int f = fork();
    if (f < 0) {
        printf("    %s\n", strerror(errno));
        exit(errno);
    }
    if (f == 0) {
        strcat(initialDirectory, "/date");

        execl(initialDirectory, "date", option, fileName, NULL);
        printf("    %s\n", strerror(errno));
    }
    else {
        wait(NULL);
//                printf("    date process complete\n");
    }
}

void removeFile() {
    char *args[10];
    fillArray(args);

    int f = fork();
    if (f < 0) {
        printf("    %s\n", strerror(errno));
        exit(errno);
    } else if (f == 0) {
        strcat(initialDirectory, "/remove");
        args[0] = initialDirectory;
        args[1] = "remove";

        execv(args[0], args);
        printf("    %s\n", strerror(errno));
    } else {
        wait(NULL);
//                printf("    remove process complete\n");
    }
}

void makeDirectory() {
    char *args[10];
    fillArray(args);

    int f = fork();
    if (f < 0) {
        printf("    %s\n", strerror(errno));
        exit(errno);
    }
    else if (f == 0) {
        strcat(initialDirectory, "/mkdir");
        args[0] = initialDirectory;
        args[1] = "mkdir";

        execv(args[0], args);
        printf("    %s\n", strerror(errno));
    }
    else {
        wait(NULL);
//                printf("    mkdir process complete\n");
    }
}

void fillArray(char **args) {
    int j = 0;
    for (int i = 2; i <= 10; ++i) {
        char *token = strtok(NULL, " ");
        if(token == NULL) {
            args[i] = NULL;
            j = i;
            break;
        }

        if(i == 2) {
            if(token[0] != '-'){
                args[2] = " ";
                i++;
            }
        }

        int l = strlen(token);
        size_t size = l + 1;
        char *c = malloc(size);
        strcpy(c, " ");

        if(token[l-1] == '\\') {
            strcpy(c, token);
            do {
                if(c[l-1] == '\\') {
                    c[l-1] = '\0';
                }
                token = strtok(NULL, " ");
                char *space = " ";
                strcat(c, space);
                strcat(c, token);
                l = strlen(c);
            } while (c[l-1] == '\\');
        }

        if(strcmp(c, " ") != 0)
            args[i] = c;
        else {
            args[i] = token;
        }
    }
    args[j-1] = strtok(args[j-1], "\n");
}

void printLogicalPath() {
    char *path = calculateLogicalPath();
    printf("    %s\n", path);
}

void addCommandToHistory(char *command ) {
    if (historyCount < HISTORY_MAX_SIZE) {
        historyArray[historyCount++] = strdup(command );
    }
    else {
        free(historyArray[0] );
        for (int index = 1; index < HISTORY_MAX_SIZE; index++) {
            historyArray[index - 1] = historyArray[index];
        }
        historyArray[HISTORY_MAX_SIZE - 1] = strdup(command );
    }
}

char* calculateLogicalPath() {
    char *path = (char*) malloc(10000*sizeof(char*));
    for (int i = 0; i < idx-1; ++i) {
        strcat(path, pathTokens[i]);
    }
    return path;
}
