#include "./headers.h"

#define BUFSIZE 4096

void cp(char* (*argv)[])
{
    char ch;
    int on = 0;
    DIR *dir;
    struct dirent *entry = NULL;
    FILE *src, *dst; // 복사하려는 source 파일과 destination 파일
    char *token, buf[BUFSIZE], newfile[BUFSIZE];
    
    getcwd(buf, BUFSIZE); // 현재 작업중인 디렉토리를 알려줌

    if((src = fopen((*argv)[1], "r")) == NULL){ // source file을 열 수 없다면
        fprintf(stderr, "cp: cannot stat '%s': No such file or directory", (*argv)[1]); // 오류메세지 출력하고
        return; // 종료
    }

    token = strtok((*argv)[2], "/");
    
    while(token != NULL) {
        
        strcpy(newfile, token);
        chdir(token);

        token = strtok(NULL, "/");
    };
    
    dir = opendir("..");
    while((entry = readdir(dir))!=NULL)
    {
        if(strcmp(entry->d_name, newfile)==0 && entry->d_type == DT_DIR){
        on = 1;
        }
    }

    if(on == 0){
        if((dst = fopen(newfile,"w")) == NULL){
            fprintf(stderr, "%s: Can't open file.\n", (*argv)[1]);
            return;
        }
    }
    else{
        if((dst = fopen((*argv)[1], "w")) == NULL){
            fprintf(stderr, "%s: Can't open file.\n", newfile);
            return;
        }
    }

    while(!feof(src)){
        ch = (char) fgetc(src);

        if(ch != EOF){
            fputc((int)ch, dst);
        }
    }

    chdir(buf);
    closedir(dir);
    fclose(src);
    fclose(dst);
}