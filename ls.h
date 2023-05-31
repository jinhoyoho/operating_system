    #include "./headers.h"

int ls(char* (*argv)[])
{
    DIR* dir = NULL; // directory 저장
    struct dirent* file_name = NULL; // 파일의 inode 번호(d_ion), 
    //현재 디렉토리 시작부터 다음 시작까지의 offset(d_off), 파일 이름(d_name)과 파일 이름의 길이(d_reclen) 저장
    struct stat file_state; // file의 상태 확인을 위한 구조체

    struct passwd* user_id; // user 정보
    struct group* group_id; // group 정보
    int len; // 길이
    char* date; // 날짜

    if ((*argv)[1] == NULL) // ls만 입력한 경우
    {
        dir = opendir("."); // 현재 directory를 연다
        while ((file_name = readdir(dir)) != NULL) // directory가 빌 때 까지 반복한다
        {
            char first_char = file_name->d_name[0]; // 첫 번째 글자 확인
            if (first_char != '.') // '.'으로 시작하는 파일은 숨김 파일이다 -> continue로 넘어감
                printf("%s   ", file_name->d_name); // 이름 출력
        }
        printf("\n");
        closedir(dir); // directory 닫기
    }
    else if (strcmp((*argv)[1], "-a") == 0) // 'ls -a'입력시 -> 숨김 속성 파일 표시
    {
        dir = opendir("."); 
        while ((file_name = readdir(dir)) != NULL)
        {
            printf("%s   ", file_name->d_name);
        }
        printf("\n");
        closedir(dir);
    }
    else if (strcmp((*argv)[1], "-l") == 0) // "-l"인 경우
    {
        dir = opendir(".");
        while ((file_name = readdir(dir)) != NULL)
        {
            stat(file_name->d_name, &file_state);

            user_id = getpwuid(file_state.st_uid); 
            group_id = getgrgid(file_state.st_gid); 

            date = ctime(&file_state.st_mtime);
            len = strlen(date); 
            int index = 0;
            char list[20];

            char first_char = file_name->d_name[0]; // 첫 번째 글자 확인
                if (first_char != '.') 
                {  // '.'으로 시작하는 파일은 숨김 파일이다 -> continue로 넘어감
                // permissions
                    switch (file_state.st_mode & S_IFMT)
                    {
                        case S_IFDIR :
                            list[index++] = 'd';
                            break;
                        default:
                            list[index++] = '-';
                            break;
                    }
                    for (int i=0; i<3; i++) {   //   파일 접근 권한 알아내기
                        if (file_state.st_mode & (S_IREAD >> i*3))
                            list[index++] = 'r';
                        else
                            list[index++] = '-';
                        if (file_state.st_mode & (S_IWRITE >> i*3))
                            list[index++] = 'w';
                        else
                            list[index++] = '-';
                        if (file_state.st_mode & (S_IEXEC >> i*3))
                            list[index++] = 'x';
                        else
                            list[index++] = '-';
                    }
                    list[index] = '\0';

                    printf("%s", list);
                // 링크 수, 소유자 이름, 그룹 이름, 파일 크기, 날짜, 제목 출력
                    printf(" %ld\t %s\t %s\t %lld\t %.*s\t %s\n", (long)file_state.st_nlink, user_id->pw_name, 
                        group_id->gr_name, (long long)file_state.st_size, len - 1, date, file_name->d_name);
                }
            }
        closedir(dir);
    }
    else if (strcmp((*argv)[1], "-al") == 0) // "-al"인 경우
    {
        dir = opendir(".");
        while ((file_name = readdir(dir)) != NULL)
        {
            stat(file_name->d_name, &file_state);

            user_id = getpwuid(file_state.st_uid); //  uid를 이용해서 user 정보를 얻음
            group_id = getgrgid(file_state.st_gid); // group id를 이용해서 group 정보를 얻음

            date = ctime(&file_state.st_mtime); // 시간
            len = strlen(date); 

            int index = 0;
            char list[20];

            char first_char = file_name->d_name[0]; // 첫 번째 글자 확인
                
            
            // permissions
                switch (file_state.st_mode & S_IFMT)
                {
                    case S_IFDIR :
                        list[index++] = 'd';
                        break;
                    default:
                        list[index++] = '-';
                        break;
                }
                for (int i=0; i<3; i++) {   // 파일 접근 권한 알아내기
                    if (file_state.st_mode & (S_IREAD >> i*3))
                        list[index++] = 'r';
                    else
                        list[index++] = '-';
                    if (file_state.st_mode & (S_IWRITE >> i*3))
                        list[index++] = 'w';
                    else
                        list[index++] = '-';
                    if (file_state.st_mode & (S_IEXEC >> i*3))
                        list[index++] = 'x';
                    else
                        list[index++] = '-';
                }
                list[index] = '\0';

                printf("%s", list);
                // 링크 수, 소유자 이름, 그룹 이름, 파일 크기, 날짜, 제목 출력
                printf(" %ld\t %s\t %s\t %lld\t %.*s\t %s\n", (long)file_state.st_nlink, user_id->pw_name, group_id->gr_name, (long long)file_state.st_size, len - 1, date, file_name->d_name);
            
            }
        closedir(dir);
    }
    else if (strcmp((*argv)[1], "-i") == 0) // -i 옵션(file의 index 출력)
    {
        dir = opendir(".");
        while ((file_name = readdir(dir)) != NULL)
        {
            stat(file_name->d_name, &file_state);

            user_id = getpwuid(file_state.st_uid); 
            group_id = getgrgid(file_state.st_gid); 

            date = ctime(&file_state.st_mtime); 
            len = strlen(date); 

            char first_char = file_name->d_name[0]; // 첫 번째 글자 확인
                if (first_char == '.') {  // '.'으로 시작하는 파일은 숨김 파일이다 -> continue로 넘어감
                    continue;
                }
            
            printf("%d %s\t", (unsigned)file_state.st_ino, file_name->d_name);
            // index 번호와 이름 출력
        }
        printf("\n");
        closedir(dir);
    }
    else if (strcmp((*argv)[1], "-ai") == 0) // -i 옵션(file의 index 출력)
    {
        dir = opendir(".");
        while ((file_name = readdir(dir)) != NULL)
        {
            stat(file_name->d_name, &file_state);

            user_id = getpwuid(file_state.st_uid);
            group_id = getgrgid(file_state.st_gid);

            date = ctime(&file_state.st_mtime);
            len = strlen(date);

            char first_char = file_name->d_name[0]; // 첫 번째 글자 확인

            printf("%d %s\t", (unsigned)file_state.st_ino, file_name->d_name);
        // index 번호와 이름 출력
        }
        printf("\n");
        closedir(dir);
    }
    else if (strcmp((*argv)[1], "-A") == 0) // "-A"인 경우 -> "."과 ".." 빼고 모두 출력 (숨김파일까지 모두 출력)
    {
        dir = opendir("."); // 현재 directory를 연다
        while ((file_name = readdir(dir)) != NULL) // directory가 빌 때 까지 반복한다
        {
            if (strcmp(".", file_name->d_name) == 0 || strcmp("..", file_name->d_name) == 0) // '.'이랑 '..'은 넘긴다
                continue;

            printf("%s   ", file_name->d_name); // 이름 출력
        }
        printf("\n");
        closedir(dir); // directory 닫기
    }
    else // 명령어가 잘 못 입력된 경우
    {
        dir = opendir((*argv)[1]);
        if (dir == NULL)
        {
            printf("failed open\n");
            return 1;
        }
        else
        {
            dir = opendir((*argv)[1]);
            while ((file_name = readdir(dir)) != NULL)
            {
                printf("%s   ", file_name->d_name);
            }
            printf("\n");
            closedir(dir);
        }
    }

    return 0;
}