#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

int rmdirs(const char *path) {
     DIR * dir_ptr = NULL;
     struct dirent *file = NULL;
     struct stat buf;
     char filename[1024]; /* 목록을 읽을 디렉토리명으로 DIR *를 return 받습니다. */
     if((dir_ptr = opendir(path)) == NULL)
     { /* path가 디렉토리가 아니라면 삭제하고 종료합니다. */
        return unlink(path);
     }
      /* 디렉토리의 처음부터 파일 또는 디렉토리명을 순서대로 한개씩 읽습니다. */
    while((file = readdir(dir_ptr)) != NULL) {
        // readdir 읽혀진 파일명 중에 현재 디렉토리를 나타네는 . 도 포함되어 있으므로 // 무한 반복에 빠지지 않으려면 파일명이 . 이면 skip 해야 함
        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
            continue;
        }
        sprintf(filename, "%s/%s", path, file->d_name);
        /* 파일의 속성(파일의 유형, 크기, 생성/변경 시간 등을 얻기 위하여 */
        if(stat(filename, &buf) == -1)
        {
            continue;
        }
        if(S_ISDIR(buf.st_mode)) {
            // 검색된 이름의 속성이 디렉토리이면 /* 검색된 파일이 directory이면 재귀호출로 하위 디렉토리를 다시 검색 */
            if(rmdirs(filename) == -1) {
                return -1;
            }
        }
        else if(S_ISREG(buf.st_mode)) {
            // 일반파일 또는 symbolic link 이면
            if(unlink(filename) == -1) {
                return -1;
            }
        }
    }
    closedir(dir_ptr);
    return rmdir(path);
 }



void rm(char* (*argv)[])
{
    DIR *dir;
    struct dirent *entry;
    int rmResult, checked;

    dir = opendir(".");
    if (strcmp((*argv)[1], "-r") == 0){
        rmdirs((*argv)[2]);

    }
    else if (strcmp((*argv)[1], "-f") == 0){
        unlink((*argv)[2]);
    }
    else if (strcmp((*argv)[1], "-rf") == 0){
        rmdirs((*argv)[2]);
    }
    else {
        while((entry = readdir(dir))!=NULL)
        {
            if(strcmp(entry->d_name, (*argv)[1]) == 0){
                //if(entry->d_type == DT_DIR){
                    rmResult = rmdir((*argv)[1]);
                    if (rmResult == -1) {
                        perror("cannot remove this directory");
                    }
                }
                else {
                    unlink((*argv)[1]);
                //}
                return;
            }
        }
        printf("cannot remove: undefined\n");

    closedir(dir);
}
}
