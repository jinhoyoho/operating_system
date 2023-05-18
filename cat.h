#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 1024
#define MAXCOUNT 10

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

        while (fgets(buf, MAX, stdin) != NULL) {
            fprintf(fp, "%s", buf);
        }
        printf("\n");
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
        int num_empty_lines = 0;  // 현재까지 저장된 빈 줄의 개수
        char *temp_buf[MAX];     // 라인을 임시로 저장하는 버퍼
        int num_lines = 0;       // 현재까지 저장된 라인의 개수
        for (i = 2; (*argv)[i] != NULL; i++) {
            if (fp = fopen((*argv)[i], "r")) {
                while (fgets(buf, MAX, fp)) {
                    // 만약 현재 라인이 빈 줄인 경우
                    if (strcmp(buf, "\n") == 0) {
                        num_empty_lines++;
                        // 이미 빈 줄이 하나 이상 저장된 상태이면 continue
                        if (num_empty_lines > 1) {
                            continue;
                        }
                    }
                    else {
                        num_empty_lines = 0;
                    }
                    // 현재 라인을 버퍼에 저장
                    temp_buf[num_lines] = malloc(sizeof(char) * strlen(buf) + 1);
                    strcpy(temp_buf[num_lines], buf);
                    num_lines++;
                }
                fclose(fp);
            }
        }
        // 버퍼에 저장된 라인을 출력
        for (i = 0; i < num_lines; i++) {
            printf("%s", temp_buf[i]);
            // 만약 현재 라인이 마지막 라인이 아니면서 다음 라인이 빈 줄이 아닌 경우
            if (i < num_lines - 1 && strcmp(temp_buf[i + 1], "\n") != 0) {
                printf("\n");
            }
        }
        printf("\n");
        // 임시 버퍼에 저장된 라인을 메모리에서 해제
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
