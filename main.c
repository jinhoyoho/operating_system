#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //chdir, 
#include <errno.h> //perrno
#include <sys/stat.h>
#include <sys/types.h>

// ���丮 ���� �Լ� ���
#include "ls.h"



#define MAXCOUNT 10


void command_split(char* command, char* (*argv)[])
{
    int i = 0;
    char* token;

    token = strtok(command, " ");
    while (token != NULL)
    {
        (*argv)[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
}

int main()

{
    int i;
    char current[1024];
    char command[1024];
    char* argv[MAXCOUNT] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, };

    do {
        getcwd(current, 1024);
        printf("%s > ", current);
        scanf("%[^\n]s", command);
        getchar();

        command_split(command, &argv);

        if (strcmp(argv[0], "ls") == 0)
        {
            ls(&argv);
        }
        else if (strcmp(argv[0], "cd") == 0)
        {
            printf("cd\n");
        }
        else if (strcmp(argv[0], "mkdir") == 0)
        {
            printf("mkdir\n");
        }
        else if (strcmp(argv[0], "cat") == 0)
        {
            printf("cat\n");
        }
        else if (strcmp(argv[0], "chmod") == 0)
        {
            printf("chmod\n");
        }
        else if (strcmp(argv[0], "chown") == 0)
        {
            printf("chown\n");
        }
        else if (strcmp(argv[0], "grep") == 0)
        {
            printf("grep\n");
        }
        else if (strcmp(argv[0], "cp") == 0)
        {
            printf("cp\n");
        }
        else if (strcmp(argv[0], "find") == 0)
        {
            printf("find\n");
        }
        else if (strcmp(argv[0], "rm") == 0)
        {
            printf("rm\n");
        }
        else if (strcmp(argv[0], "exit") == 0)
        {
            break;
        }
        else {
            printf("error: not valid command\n\n");
        }

        for (i = 0; i < MAXCOUNT; i++)
        {
            argv[i] = NULL;
        }
    } while (1);

    return 0;
}
