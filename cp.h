#include "./headers.h"

void cp(char* (*argv)[])
{
    char ch;
    int on = 0;
    DIR *dir;
    struct dirent *entry = NULL;
    FILE *src, *dst;
    char *token, buf[1024], newfile[1024];
    getcwd(buf, 1024);

    if((src = fopen((*argv)[1], "r")) == NULL){
        fprintf(stderr, "%s: Can't open file.\n", (*argv)[1]);
        return;
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