#include "./headers.h"
#include <stdbool.h>

#define BUFSIZE 4096

// cp는 src를 dst에 복사하는 명령어
// 만약 dst가 file이면 덮어쓰기, directory면 해당 directory에 src이름으로 파일을 생성한다.

void cp(char* (*argv)[]) // 옵션없이 하나만 구현
{
    bool is_dir = false; // directory인지 file인지 구분하는 변수
    struct dirent *entry = NULL;
    struct stat path_stat;
    FILE *src, *dst; // 복사하려는 src(file), dst(directory or file)
    char current[BUFSIZE], newfile_name[BUFSIZE], path[BUFSIZE], ch;

    getcwd(current, BUFSIZE); // 현재 작업중인 디렉토리를 알려줌

    if((src = fopen((*argv)[1], "r")) == NULL) // source file을 열 수 없다면
    { 
        fprintf(stderr, "cp: cannot stat '%s': No such file or directory\n", (*argv)[1]); // 오류메세지 출력하고
        return; // 종료
    }

    // 새로 정할 파일 이름
    char input[BUFSIZE];
    strncpy(input, (*argv)[2], sizeof(input)-1);
    input[sizeof(input)-1] = '\0';

    char* lastSlash = strrchr(input, '/'); // 문자열에서 마지막 슬래시 위치 찾기
    memset(path, 0, sizeof(path));

    if (lastSlash != NULL) {
        strcpy(newfile_name, lastSlash + 1); // 파일 이름 복사
        strncpy(path, (*argv)[2], lastSlash - input); // 경로 같이 이동
    } else {
        strcpy(newfile_name, input); // 슬래시가 없는 경우 전체 문자열을 파일 이름으로 간주
    }
    // test_path엔 이전까지 경로가 들어감
    chdir(path);


    // directory인지 확인하는 코드
    if (stat((*argv)[2], &path_stat) == 0)
    {
        if (S_ISDIR(path_stat.st_mode)){
            is_dir = true;
        }
    }

    if (is_dir)
    {
        chdir(newfile_name); // directory이므로 해당 directory로 이동
        if ((dst = fopen((*argv)[1], "w")) == NULL) // src를 dst directory로 복사함
        {
            fprintf(stderr, "%s: Can't open file.\n", newfile_name);
            return;
        }
    }
    else 
    {
        if((dst = fopen(newfile_name, "w")) == NULL) // newfile_name 이름으로 파일을 생성함
        {
            fprintf(stderr, "%s: Can't open file.\n", (*argv)[1]);
            return;
        }
    }
    
    // 파일 복사
    while ((ch = fgetc(src)) != EOF) { 
        fputc(ch, dst);
    }

    chdir(current); // 현재 위치로 다시 복귀

    fclose(src);
    fclose(dst); // 파일 모두 닫기
}
