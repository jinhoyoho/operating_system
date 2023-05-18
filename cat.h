#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#define MAX 1024
#define MAXCOUNT 10

int flag = false;

void handle_sigint(int sig) {
    flag = true; // flag를 true로 변경
}

void cat(char* (*argv)[])
{
	char ch;
	int num = 1;
	char buf[MAX];
	FILE *fp;

    if (strcmp((*argv)[1], ">") == 0) {
        fp = fopen((*argv)[2], "w");

        if (fp == NULL) {
            perror("can't open");
            return;
        }
        signal(SIGINT, handle_sigint); // ^C를 입력받는 경우 예외 처리

        while (fgets(buf, MAX, stdin) != NULL) {
            fprintf(fp, "%s", buf);
            if (flag == true){ // ^C가 입력받아진 경우
                flag = false; // 원래대로 만들고
                break; // 반복문 탈출
            }
        }
        fclose(fp);
    }
    else if(strcmp((*argv)[1], "-n")==0)
        {
            if(fp=fopen((*argv)[2], "r"))
            {
                printf("%3d| ",num);
                do
                {
                    ch = fgetc(fp);
                    printf("%c", ch);
                    if(ch == '\n'){
                        printf("%3d| ", ++num);
                    }
                }while(ch != EOF);
                printf("\n");
                fclose(fp);
            }
        }
    else if(strcmp((*argv)[1], "-E")==0)
    {
        if(fp=fopen((*argv)[2], "r"))
        {
            while(fgets(buf, MAX, fp))
            {
                buf[strlen(buf)-1] = '\0';
                printf("%s", buf);
                printf("$\n");
            }
            fclose(fp);
        }
    }
        else if(strcmp((*argv)[1], "-s")==0) {
        int i;
        int num_empty_lines = 0;  
        char *temp_buf[MAX];     
        int num_lines = 0;       
        for (i = 2; (*argv)[i] != NULL; i++) {
            if (fp = fopen((*argv)[i], "r")) {
                while (fgets(buf, MAX, fp)) {
                    if (strcmp(buf, "\n") == 0) {
                        num_empty_lines++;
                        if (num_empty_lines > 1) {
                            continue;
                        }
                    }
                    else {
                        num_empty_lines = 0;
                    }
                    temp_buf[num_lines] = malloc(sizeof(char) * strlen(buf) + 1);
                    strcpy(temp_buf[num_lines], buf);
                    num_lines++;
                }
                fclose(fp);
            }
        }
      
        for (i = 0; i < num_lines; i++) {
            printf("%s", temp_buf[i]);
           
            if (i < num_lines - 1 && strcmp(temp_buf[i + 1], "\n") != 0) {
                printf("\n");
            }
        }
        printf("\n");
        for (i = 0; i < num_lines; i++) {
            free(temp_buf[i]);
        }
    }
    else {
        int i;
        for (i = 1; (*argv)[i] != NULL; i++) {
            if (fp = fopen((*argv)[i], "r")) {
                while (fgets(buf, MAX, fp)) {
                    printf("%s", buf);
                }
                printf("\n");
                fclose(fp);
            }
        }
    }
}
