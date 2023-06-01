#include "./headers.h"

mode_t convert_mode_str_to_oct_(const char* target);

int chmod_(int argc, char *argv[])
{
    struct stat file_exist;
    char *token;

    if (argc<3){
        printf("Error\n");  //인자가 3개 입력되어야 하는데 적게 입력될 경우.
        return -1;
    }
    if(stat(argv[2], &file_exist) == -1){
        printf("Can't access to %s\n", argv[2]); //f1파일이 존재하지 않으면 오류
        return -1;
    }

    struct stat buf;
    if(argv[1][0]>='0' && argv[1][0]<='9'){
        mode_t mode;
        token = strtok(argv[1], " ");
        if ((int)(mode = convert_mode_str_to_oct(token)) == -1) {
            fprintf(stderr, "Invalid parameter\n");
            return -1;
        }

        chmod(argv[2], mode);
        /*if(chmod(argv[2], mode)==-1)
            printf("   ");*/
    }
    else{
        if(stat(argv[2], &buf)== -1){ //lstat을 stat으로 바꿈. //argv[2]의 파일상태를 buf 구조체에 넣어줌.
            printf("read data error");
            return -1;
        }

        char *token = strtok(argv[1], ", ");
        while (token != NULL) {
            switch(token[0]){
                case 'u':
                    switch(token[1]){
                    case '+':
                        for (int i = 2; token[i] != '\0'; i++) {
                            switch(token[i]){
                                case 'x':
                                    buf.st_mode |= S_IXUSR;
                                    break;
                                case 'w':
                                    buf.st_mode |= S_IWUSR;
                                    break;
                                case 'r':
                                    buf.st_mode |= S_IRUSR;
                                    break;
                            }
                        }
                        break;
                    case '-':
                        for (int i = 2; token[i] != '\0'; i++) {
                            switch(token[i]){
                                case 'x':
                                    buf.st_mode &= ~S_IXUSR;
                                    break;
                                case 'w':
                                    buf.st_mode &= ~S_IWUSR;
                                    break;
                                case 'r':
                                    buf.st_mode &= ~S_IRUSR;
                                    break;
                            }
                        }
                        break;
                    case '=':
                        buf.st_mode = 0;
                        for (int i = 2; token[i] != '\0'; i++) {
                            switch(token[i]){ // 수정해야 함
                                case 'x':
                                    buf.st_mode |= S_IXUSR;
                                    break;
                                case 'w':
                                    buf.st_mode |= S_IWUSR;
                                    break;
                                case 'r':
                                    buf.st_mode |= S_IRUSR;
                                    break;
                            }
                        }
                        break;
                }
                    break;
                case 'g':
                    switch(token[1]){
                    case '+':
                        for (int i = 2; token[i] != '\0'; i++) {
                            switch(token[i]){
                                case 'x':
                                    buf.st_mode |= S_IXGRP;
                                    break;
                                case 'w':
                                    buf.st_mode |=S_IWGRP;
                                    break;
                                case 'r':
                                    buf.st_mode |=S_IRGRP;
                                    break;
                            }
                        }
                        break;
                    case '-':
                        for (int i = 2; token[i] != '\0'; i++) {
                            switch(token[i]){
                                case 'x':
                                    buf.st_mode &= (~S_IXGRP);
                                    break;
                                case 'w':
                                    buf.st_mode &= (~S_IWGRP);
                                    break;
                                case 'r':
                                    buf.st_mode &= (~S_IRGRP);
                                    break;
                            }
                        }
                        break;
                }
                    break;
                case 'o':
                    switch(token[1]){
                    case '+':
                        for (int i = 2; token[i] != '\0'; i++) {
                            switch(token[i]){
                                case 'x':
                                    buf.st_mode |= S_IXOTH;
                                    break;
                                case 'w':
                                    buf.st_mode |=S_IWOTH;
                                    break;
                                case 'r':
                                    buf.st_mode |=S_IROTH;
                                    break;
                            }
                        }
                        break;
                    case '-':
                        for (int i = 2; token[i] != '\0'; i++) {
                            switch(token[i]){
                                case 'x':
                                    buf.st_mode &= (~S_IXOTH);
                                    break;
                                case 'w':
                                    buf.st_mode &= (~S_IWOTH);
                                    break;
                                case 'r':
                                    buf.st_mode &= (~S_IROTH);
                                    break;
                            }
                        }
                        break;
                }
                    break;
            }

            token = strtok(NULL, ", ");
        }
    }
    if(chmod(argv[2], buf.st_mode)==-1)
        printf("   ");
    return 0;
}

mode_t convert_mode_str_to_oct_(const char* target) {

    mode_t result;
    size_t len;

    len = strlen(target);
    if (strlen(target) >= 4)
        return -1;

    result = 0;
    for (int i = 0; target[i] != '\0'; i++) {
        if (target[i] >= '0' && target[i] <= '7')
            result |= (target[i] - '0') << (len - i - 1) * 3;
        else
            return -1;
    }

    return result;
}
