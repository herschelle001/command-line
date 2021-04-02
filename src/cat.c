#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

extern int errno;

int main(int argc, char *argv[]) {
    FILE *file;
    char line[100];
    file = fopen(argv[1], "r");
    char *option = argv[2];
    char *fileName = argv[3];

    if (strncmp(option, " ", 1) == 0) {
        while (fscanf(file, "%[^\n]\n", line) != EOF) {
            printf("    %s\n", line);
        }
    }
    else if(strcmp(option, "-n") == 0) {
        int i=1;
        while(fscanf(file,"%[^\n]\n",line) != EOF){
            printf("    %d  ", i++);
            printf("    %s\n", line);
        }
    }
    else if(strcmp(option, ">") == 0) {
        FILE *fPtr1, *fPtr2;
        fPtr1 = fopen(argv[1], "r");
        fPtr2 = fopen(fileName, "w");
        if(fPtr2 == NULL){
            printf("    %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        char c = fgetc(fPtr1);
        while (c != EOF) {
            fputc(c, fPtr2);
            c = fgetc(fPtr1);
        }

        fclose(fPtr1);
        fclose(fPtr2);
        printf("    Contents copied to %s\n", argv[3]);

    }
    else if(strcmp(option, "-E") == 0) {
        while(fscanf(file,"%[^\n]\n",line) != EOF){
            printf("    %s", line);
            printf("$\n");
        }
    }
    else {
        printf("Invalid option\n");
    }
    fclose(file);
}
