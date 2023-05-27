#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //chdir, 
#include <errno.h> //perrno
#include <sys/stat.h>
#include <sys/types.h>

#include "ls.h"
#include "cat.h"
#include "cp.h"
#include "cd.h"
//#include "mkdir.h"
#include "grep.h"
#include "rm.h"
#include "chmod.h"

#define MAXCOUNT 10


int command_split(char* command, char* (*argv)[])
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

    return i;
}

int main()

{
    int i;
    char current[1024];
    char command[1024];
    char* argv[MAXCOUNT] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, };

    int argc;
  
    do {
        getcwd(current, 1024);
        printf("%s > ", current);
        scanf("%[^\n]s", command);
        getchar();

        argc = command_split(command, &argv);

        if (strcmp(argv[0], "ls") == 0)
        {
            ls(&argv);
        }
        else if (strcmp(argv[0], "cd") == 0)
        {
            cd(current, argv+1);
        }
        else if (strcmp(argv[0], "mkdir") == 0)
        {
            //mkdir(argc, argv);
        }
        else if (strcmp(argv[0], "cat") == 0)
        {
            cat(&argv);
        }
        else if (strcmp(argv[0], "chmod") == 0)
        {
            chmod_(argc, argv);
        }
        else if (strcmp(argv[0], "chown") == 0)
        {
            printf("chown\n");
        }
        else if (strcmp(argv[0], "grep") == 0)
        {
            if (argv[1] != NULL && argv[2] != NULL) {
                int show_line_numbers = 0;
                int invert_match = 0;
                int ignore_case = 0;
                int arg_index = 1;

                if (strcmp(argv[arg_index], "-n") == 0) {
                    show_line_numbers = 1;
                    arg_index++;
                }

                if (strcmp(argv[arg_index], "-v") == 0) {
                    invert_match = 1;
                    arg_index++;
                }

                if (strcmp(argv[arg_index], "-i") == 0) {
                    ignore_case = 1;
                    arg_index++;
                }

                grep(argv[arg_index], argv[arg_index + 1], show_line_numbers, invert_match, ignore_case);
            } else {
                printf("grep ���ɾ��� ����: grep [-n] [-v] [-i] <����> <���ϸ�>\n");
            }
        }
        else if (strcmp(argv[0], "cp") == 0)
        {
            cp(&argv);
        }
        else if (strcmp(argv[0], "find") == 0)
        {
            printf("find\n");
        }
        else if (strcmp(argv[0], "rm") == 0)
        {
            char* path = argv[0] + 3; // "rm" 이후의 경로 부분
            if (rmdirs(path) == 0) {
                printf("The file or directory has been successfully deleted.\n");
            else {
                printf("File or directory delection failed: %s\n", path);
            }
            }
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
