#include "./headers.h"
#include <stdbool.h>

#define BUFSIZE 4096

void cp(char* (*argv)[]) // 옵션없이 하나만 구현
{
    int is_dir = 0;
    DIR *dir;
    struct dirent *entry = NULL;
    FILE *src, *dst; // 복사하려는 source 파일과 destination 파일
    char *flag, buf[BUFSIZE], newfile_name[BUFSIZE];
    char * current, ret;
    
    current = getcwd(buf, BUFSIZE); // 현재 작업중인 디렉토리를 알려줌

    if((src = fopen((*argv)[1], "r")) == NULL) // source file을 열 수 없다면
    { 
        fprintf(stderr, "cp: cannot stat '%s': No such file or directory", (*argv)[1]); // 오류메세지 출력하고
        return; // 종료
    }

    flag = strtok((*argv)[2], "/"); // dst 파일의 경로를 '/'를 기준으로 나눔
    
    while(flag != NULL) 
    {
        strcpy(newfile_name, flag); // newfile_name에 마지막 이름 저장
        chdir(flag); // 경로 변경
        flag = strtok(NULL, "/"); // 다음 문자열을 잘라서 포인터 반환
    };
    
    dir = opendir("..");

    while((entry = readdir(dir))!=NULL)
    {
        if(strcmp(entry->d_name, newfile_name)==0 && entry->d_type == DT_DIR) // directory인 경우
            is_dir = true;
    }

    if (is_dir == false) // 2번째 인자가 파일인 경우
    {
        dst = fopen(newfile_name,"w"); // newfile_name이란 이름으로 파일을 생성함
        if(dst == NULL)
        {
            fprintf(stderr, "%s: Can't open file.\n", (*argv)[1]);
            return;
        }
    }
    else // 2번째 인자가 directory인 경우
    {
        dst = fopen((*argv)[1], "w"); // 기존에 있는 파일을 새로운 directory로 이동함
        if (dst == NULL)
        {
            fprintf(stderr, "%s: Can't open file.\n", newfile_name);
            return;
        }
    }

    while(ret = fread(buf, 1, sizeof(buf), src))
    {
        fwrite(buf, 1, ret, dst);
    }


    chdir(current); // 현재 위치로 다시 복귀

    fclose(src);
    fclose(dst);
}
