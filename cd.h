#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 1024


void cd(char current[], char *path[]) {
    char new_directory[MAX];

    if (path[0][0] == '/') {
        strncpy(new_directory, path[0], sizeof(new_directory));
    } else {
        strncpy(new_directory, current, sizeof(new_directory));
        strncat(new_directory, "/", sizeof(new_directory) - strlen(new_directory) - 1);
        strncat(new_directory, path[0], sizeof(new_directory) - strlen(new_directory) - 1);
    }

    if (chdir(new_directory) == 0) {
        getcwd(current, MAX);
    } else {
        printf("cd: %s: No such file or directory \n", new_directory); // printf문장이 깨져서 임의로 넣음 -> 추후에 수정 필요!
    }
}

