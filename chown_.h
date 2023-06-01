#include "./headers.h"

int chown_(int argc, char* argv[])
{
    struct stat file_exist;

    if (argc < 3) {
        printf("Error\n"); //인자가 3개 입력되어야 하는데 적게 입력될 경우.
        return -1;
    }
    if (stat(argv[2], &file_exist) == -1) {
        printf("Can't access to %s\n", argv[2]); //파일이 존재하지 않으면 오류
        return -1;
    }


    char* k;
    int location;
    k = strchr(argv[1], ':'); // argv[1]에서 :를 찾으면 그 위치의 주소가 k에 저장됨.
    location = (int)(k - argv[1]); // ':'의 위치의 주소에서 argv[1]의 시작 주소 빼어 몇번째 있는지 알 수 있음.
    int length = sizeof(argv[1]) / sizeof(argv[1][0]); //argv[1]의 크기 구하기
    char u_name[length];
    char g_name[length];

    for (int i = 0; i < location; i++) {
        u_name[i] = argv[1][i];
    }
    for (int j = location + 1; j < length; j++) {
        g_name[j - location - 1] = argv[1][j];
    }


    if (k == NULL) {    //:가 없다는 뜻이므로 user만 변경한다
        //이 파일에서 소유자(user)를 내가 입력한 소유자로 변경한다.
        if (chown(argv[2], argv[1], -1) == -1)
            printf("Fail");
    }

    else if (location == 0) {  //:를 찾았고 얘는 argv[1]의 0번째에(맨앞에) 위치. 즉 그룹만 변경한다.
        //이 파일에서 그룹을 내가 입력한 그룹으로 변경한다.
        if (chown(argv[2], -1, g_name) == -1)
            printf("Fail");
    }
    else {   //user와 group 모두 변경.
        //이 파일에서 소유자와 그룹을 모두 내가 입력한대로 변경한다.
        if (chown(argv[2], u_name, g_name) == -1)
            printf("Fail");
    }

    return 0;
}
#pragma once
