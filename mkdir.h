//
//  mkdir_.h
//  OS_Test
//
//  Created by 최정흠 on 2023/05/24.
//

#include <stdio.h> // mkdir
#include <stdlib.h> // mkdir
#include <string.h> // mkdir
#include <unistd.h> // mkdir
#include <sys/stat.h> // mkdir
#include <sys/syscall.h> // mkdir
#include <sys/types.h> // mkdir
#include <fcntl.h> // mkdir
//#include <pthread/pthread.h> // mkdir
#include <pthread.h> // mkdir
#include <errno.h> // mkdir

void mkdir_(int argc, char *argv[]);
int build_dir(char *path, mode_t mode);
mode_t convert_mode_str_to_oct(const char *target);
int find_brace_pattern(char *target, int *start, int *end);
int build_brace_dir(mode_t mode, char *real_path, int find_dirname, int start, int end);
int isAbsolutePath(const char *path);

void *pthread_mkdir_func(void *arg_);

typedef struct {
    int dirfd;
    char dir_name[1024];
    mode_t mode;
} ThreadArgs;

void mkdir_(int argc, char *argv[]) {
    
    int flag_p, flag_v;
    int returned_opt;
    int success;
    int dirfd;
    int i;
    int index;
    int path_len;
    int start, end;
    char *mode;
    char real_path[1024];
    mode_t old_mask, new_mask;
    mode_t mode_oct;
    pthread_t *pthread;
    int res;
    
    // Initialize getopt's extern variables
    optind = 1;
    optopt = '\0';
    optarg = NULL;
    // Initialize mode variables
    mode = NULL;
    mode_oct = flag_p = flag_v = 0;
    
    // Parsing option
    while ((returned_opt = getopt(argc, argv, "m:pv")) != -1)
        switch (returned_opt) {
            case 'm':
                mode = optarg; // get mode input
                break;
            case 'p':
                flag_p = 1;
                break;
            case 'v':
                flag_v = 1;
                break;
            case '?':
                printf("usage: mkdir [-pv] [-m mode] directory_name ...\n");
                return;
        }
    
    // Move pointer to path
    argc -= optind;
    argv += optind;
    
    if (*argv == NULL) {
        printf("usage: mkdir [-pv] [-m mode] directory_name ...\n");
        return;
    }
    
    // set file authority
    if (mode == NULL) {
        mode_oct = S_IRWXU | S_IRWXG | S_IRWXO; // Set mode 0777
    } else {
        if ((int)(mode_oct = convert_mode_str_to_oct(mode)) == -1) {
            fprintf(stderr, "Invalid parameter\n");
            return;
        }
        printf("%o\n", mode_oct); // Test
    }
    
    /**
     * Relative path : start with directory name or './' '../'
     * Absolute path : start with '/'
     */
    
    if (argc != 1) // use pthread
        pthread = (pthread_t *) malloc(sizeof(pthread_t) * argc);
    else
        pthread = NULL;
    
    old_mask = umask(022);
    for ( ; argc > 0; argc--, argv++) {
        
        /**
         * 상대 경로인가? 절대 경로인가?
         * => 절대 경로로 변환
         * 밑에 경로부터 dir이 존재하는지 안 하는지 확인한다.
         * => p flag라면 dir 존재 여부말고 그냥 다 만들어버린다.
         * => 이미 존재하는 dir일 수도 있으니, 반복문으로 dir인 부분 체크 뒤, 그 부분부터 만든다.
         * => p flag가 아니라면 dir 존재 여부 체크 뒤, 그 부분에 만든다.
         *
         */
        success = 0;
        
        if (!isAbsolutePath(*argv))
            realpath(*argv, real_path); // transfrom to real path
        else
            strcpy(real_path, *argv); // NEED MODIFY
        
        if (flag_p) {
            for (index = strlen(real_path) - 1; real_path[index - 1] != '/'; index--);
            real_path[index] = '\0';
            strcat(real_path, *argv); // 이 부분 반드시 수정하기
            success = build_dir(real_path, mode_oct);
        } else {                        // Case of don't need to make subdirectory
            path_len = (int)strlen(real_path);
            for (i = path_len - 1; i >= 0; i--)
                if (real_path[i] == '/') {
                    real_path[i] = '\0';
                    i++;
                    break;
                }
            if (find_brace_pattern(real_path + i, &start, &end)) {
                success = build_brace_dir(mode_oct, real_path, i, start, end);
            } else {                    // input dosen't have dirname{%d..%d} pattern
                if ((dirfd = open(real_path, O_RDONLY)) != -1) {
                    res = syscall(SYS_mkdirat, dirfd, real_path + i, mode_oct);
                    if (res != -1)
                        success = 1;
                    close(dirfd);
                }
                else
//                    fprintf(stderr, "open funct failed\n");
                    perror("syscall(SYS_mkdirat)");
            }
        }
        
        if (flag_v && success) // mode v
            printf("%s\n", *argv);
    }
    
    umask(old_mask);
    
    if (pthread != NULL)
        free(pthread);
   
    return;
}

int build_dir(char *path_org, mode_t mode) {
    
    struct stat path_stat;
    int dirfd;
    int len;
    char *path, *i;

    path = path_org;
    
    len = (int)strlen(path_org);
    if (path_org[len - 1] != '/') {
        path_org[len] = '/';
        path_org[len + 1] = '\0';
    }
    
    if (*path == '/')
        path++;
    for ( ; ; path++) {
        if (*path == '\0')
            break;
        else if (*path == '/')
            *path = '\0';
        else
            continue;
        
        if (stat(path_org, &path_stat) == -1) { // path is not exist
            if (errno == ENOENT) {
                for (i = path - 1; *(i - 1) != '/'; i--);
                *(i - 1) = '\0';
                if ((dirfd = open(path_org, O_RDONLY)) != -1) {
                    syscall(SYS_mkdirat, dirfd, i, mode);
                    close(dirfd);
                }
                *(i - 1) = '/';
            } else
                return 1;
        } else {                                // path is exist
            if (!S_ISDIR(path_stat.st_mode))
                fprintf(stderr, "%s:No such file or directory\n", path_org);
            else
                fprintf(stderr, "%s:File exists\n", path_org);
        }
        *path = '/';
    }
    
    return 0;
}

mode_t convert_mode_str_to_oct(const char *target) {
    
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

int find_brace_pattern(char *target, int *start, int *end) {
    
    char temp[1024];

    // parsing from "abc{1..3}" for get baseName, start, end
    if (sscanf(target, "%1024[^{]{%d..%d}", temp, start, end) == 3 && target[strlen(target) - 1] == '}')
        return 1;
    else
        return 0;
}

int build_brace_dir(mode_t mode, char *real_path, int find_dirname, int start, int end) {
    
    int dirfd;
    int brc_pt_cnt;
    pthread_t *pthread, *brc_pt_ptherad;
    ThreadArgs *args;
    char dir_name[1024];
    int res;
    
    if ((brc_pt_cnt = end - start + 1) <= 0) {
        fprintf(stderr, "Invalid dirname{%d..%d}", start, end);
        return 0;
    }
        
    brc_pt_ptherad = (pthread_t *) malloc(sizeof(pthread_t) * brc_pt_cnt);
    args = (ThreadArgs *) malloc(sizeof(ThreadArgs) * brc_pt_cnt);
    
    if ((dirfd = open(real_path, O_RDONLY)) == -1) {
        fprintf(stderr, "open funct failed\n");
        return 0;
    }
    
    strcpy(dir_name, strtok(real_path + find_dirname, "{"));
    for (int j = 0; j < brc_pt_cnt; j++) {
        
        args[j].dirfd = dirfd;
        sprintf(args[j].dir_name, "%s%d", dir_name, start + j);
        args[j].mode = mode;
        printf("%s\n", args[j].dir_name);
        res = pthread_create(&brc_pt_ptherad[j], NULL, pthread_mkdir_func, (void *)&args[j]);
        if (res != 0) {
            perror("Failed to create thread");
            return 0;
        }
    }
    
    res = 0;
    for (int j = 0; j < brc_pt_cnt; j++) {
        res = pthread_join(brc_pt_ptherad[j], NULL);
        if (res != 0) {
            perror("Failed to join thread");
            return 0;
        }
    }
    
    close(dirfd);
    free(brc_pt_ptherad);
    free(args);
    
    return 1; // return success
}

void *pthread_mkdir_func(void *arg_) {
    
    ThreadArgs *arg = (ThreadArgs *) arg_;
    int result;
    
    result = syscall(SYS_mkdirat, arg->dirfd, arg->dir_name, arg->mode);
    if (result == -1) {
        perror("failed pthread");
        pthread_exit((void *)1);
    }
    
    pthread_exit(NULL);
}

int isAbsolutePath(const char *path) {
    return (path[0] == '/');
}
