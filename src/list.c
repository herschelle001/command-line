#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/dir.h>
#include <errno.h>

extern int errno;


int file_select(struct direct *entry) {
    if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
        return 0;
    else
        return 1;
}

int main(int argc, char *argv[]) {
    struct direct **files = (struct direct **) argv[2];
    char *option = argv[1];
    char path[100];
    if (!getcwd(path, 100)) {
        printf("Error getting path/n");
        printf("    %s\n", strerror(errno));
    }

    printf("    Current Working Directory = %s\n", path);
    int count = scandir(path, &files, (int (*)(const struct dirent *)) file_select, alphasort);
    if (count <= 0) {
        printf("    No files in this directory\n");
    }

//    printf("Number of files = %d\n",count);

    if (strncmp(option, "-a", 2) == 0) {
        for (int i = 1; i < count + 1; ++i)
            printf("    %s  ", files[i - 1]->d_name);
    }
    else if (strncmp(option, "-1", 2) == 0) {
        for (int i = 1; i < count + 1; ++i)
            if (files[i - 1]->d_name[0] != '.') {
                printf("    %s\n", files[i - 1]->d_name);
            }
    }
    else if (strncmp(option, " ", 1) == 0) {
        for (int i = 1; i < count + 1; ++i)
            if (files[i - 1]->d_name[0] != '.') {
                printf("    %s  ", files[i - 1]->d_name);
            }
    }
    else {
        printf("    Invalid option\n");
    }

    printf("\n");
}

