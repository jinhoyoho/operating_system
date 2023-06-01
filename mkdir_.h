//
//  main.c
//  OS_Test
//
//  Created by 최정흠 on 2023/05/12.
//
#include "./headers.h"

#define MAX 1024

void mkdir_(int argc, char *argv[]);
int build_dir(char *path, mode_t mode);
int find_brace_pattern(char *target, int *start, int *end);
int build_brace_dir(mode_t mode, char *real_path, int find_dirname, int start, int end);
mode_t convert_mode_str_to_oct(const char *target);
int isAbsolutePath(const char *path);
void *pthread_mkdir_func(void *arg_);
void print_log(FILE *dirFile, char *dir_name);

typedef struct {
    int dirfd;
    char dir_name[MAX];
    mode_t mode;
} ThreadArgs;

int v_flag = 0;
void mkdir_(int argc, char *argv[]) {
    
    int p_flag;
    int returned_opt;
    int success;
    int dirfd;
    int i;
    int index;
    int path_len;
    int start, end;
    char *mode;
    char real_path[MAX];
    mode_t old_mask;
    mode_t mode_oct;
    int res;
    
    // Initialize getopt's external variables
    optind = 1;
    optopt = '\0';
    optarg = NULL;
    // Initialize mode variables
    mode = NULL;
    mode_oct = p_flag = v_flag = 0;
    
    // Parsing options using getopt()
    while ((returned_opt = getopt(argc, argv, "m:pv")) != -1)
        switch (returned_opt) {
            case 'm':
                mode = optarg; // Get mode input
                break;
            case 'p':
                p_flag = 1;
                break;
            case 'v':
                v_flag = 1;
                break;
            case '?':
            default:
                fprintf(stderr, "usage: mkdir [-pv] [-m mode] directory_name ...\n");
                return;
        }
    
    // Move the pointer to the path
    argc -= optind;
    argv += optind;
    
    if (*argv == NULL) {
        fprintf(stderr, "usage: mkdir [-pv] [-m mode] directory_name ...\n");
        return;
    }
    
    // set file mode
    if (mode == NULL) {
        mode_oct = S_IRWXU | S_IRWXG | S_IRWXO; // Set mode 0777
    } else {
        if ((int)(mode_oct = convert_mode_str_to_oct(mode)) == -1) {
            fprintf(stderr, "invaild file mode: %s\n", mode);
            return;
        }
    }
    
    old_mask = umask(022);
    for ( ; argc > 0; argc--, argv++) {
        
        success = 0;
        
        // Check if the directory name is a relative path
        if (!isAbsolutePath(*argv))
            realpath(*argv, real_path); // transfrom to real path
        else if (!p_flag)
            strcpy(real_path, *argv);
        
        if (p_flag) {
            for (index = (int)strlen(real_path) - 1; real_path[index - 1] != '/'; index--);
            real_path[index] = '\0';
            strcat(real_path, *argv);
            success = build_dir(real_path, mode_oct);
        } else { // Case of don't need to make subdirectory
            path_len = (int)strlen(real_path);
            for (i = path_len - 1; i >= 0; i--)
                if (real_path[i] == '/') {
                    real_path[i] = '\0';
                    i++;
                    break;
                }
            // Find dirname{%d..%d} pattern
            if (find_brace_pattern(real_path + i, &start, &end)) {
                success = build_brace_dir(mode_oct, real_path, i, start, end);
            } else { // input dosen't have dirname{%d..%d} pattern
                if ((dirfd = open(real_path, O_RDONLY)) != -1) {
                    res = syscall(SYS_mkdirat, dirfd, real_path + i, mode_oct);
                    if (res < 0)
                        perror("mkdir");
                    else {
                        if (v_flag)
                            printf("%s/%s\n", real_path, real_path + i);
                        
                        FILE* dirFile = fopen("log.txt", "a+");
                        if (dirFile == NULL)
                            perror("can't open log.txt");
                        else
                            print_log(dirFile, real_path + i);
                    }
                    
                    close(dirfd);
                }
                else
                    perror("mkdir");
            }
        }
    }
    
    umask(old_mask);
    
    return;
}

/**
 *  Builds directories based on the given path.
 *
 *  Inputs:
 *      char *path_org
 *          The path where directories will be created.
 *
 *      mode_t mode
 *          The file permission mode for the directories to be created.
 *
 *  Returns:
 *      Returns 1 if the directories are created successfully.
 *      Returns 0 if an error occurs during the creation of directories.
 *
 *  Notes:
 *      - This function creates directories based on the given path.
 *      - The 'path_org' parameter indicates the path where directories will be created.
 *      - The 'mode' parameter specifies the file permission mode for the directories.
 *      - If intermediate directories in the path do not exist, they will be created.
 *      - If the path already exists as a directory, an error message will be printed.
 */
int build_dir(char *path_org, mode_t mode) {
    
    // path_stat : is structure to store file/directory information
    struct stat path_stat;
    // dirfd : is file descriptor for the directory.
    int dirfd;
    // Pointers to manipulate the path string
    char *path, *i, *temp;
    // res : is syscall function's return value
    int res;
    char *dir_name;
    
    // Set the initial path pointer.
    path = path_org;
    
    // Trim trailing slashes from the path.
    temp = strrchr(path_org, '\0');
    while (--temp > path_org && *temp == '/')
        *temp = '\0';
    
    int len = (int)strlen(path_org);
    if (path_org[len - 1] != '/') {
        path_org[len] = '/';
        path_org[len + 1] = '\0';
    }
    
    // Skip leading slash if present.
    if (*path == '/')
        path++;
    // Iterate through the path components.
    for ( ; ; path++) {
        if (*path == '\0')
            break;
        else if (*path == '/')
            *path = '\0';
        else
            continue;
        
        res = 0;
        // Check if the path component exists
        if (stat(path_org, &path_stat) == -1) { // Path does not exist
            if (errno == ENOENT) {
                // Find the previous slash
                for (i = path - 1; *(i - 1) != '/'; i--);
                *(i - 1) = '\0';
                // Open the parent directory and create the directory component
                if ((dirfd = open(path_org, O_RDONLY)) != -1) {
                    res = syscall(SYS_mkdirat, dirfd, i, mode);
                    close(dirfd);
                }
                *(i - 1) = '/';
                if (res < 0)
                        perror("mkdir");
                else {
                    if (v_flag)
                        printf("%s\n", path_org);
                    
                    FILE* dirFile = fopen("log.txt", "a+");
                    if (dirFile == NULL)
                        perror("can't open log.txt");
                    else
                        print_log(dirFile, i);
                }
            } else
                return 0;
        } else { // Path does exist
            if (!S_ISDIR(path_stat.st_mode))
                fprintf(stderr, "%s:No such file or directory\n", path_org);
            else
                dir_name = path + 1;
        }
        *path = '/';
    }
    
    return 1;
}

/**
 *  Builds multiple directories with a brace pattern.
 *  The function creates directories based on the given brace pattern
 *  and a range of numbers.
 *
 *  Inputs:
 *      mode_t  mode
 *         The file permission mode for the directories to be created.
 *
 *      char  *real_path
 *         The path where the directories will be created.
 *
 *      int find_dirname
 *         The index of the directory name in the 'real_path' string.
 *
 *      int start
 *          The starting number of the range for creating directories.
 *
 *      int end
 *          The ending number of the range for creating directories.
 *
 *  Return:
 *      Returns 1 if the directories are created successfully.
 *      Returns 0 if an error occurs during the creation of directories.
 *
 *  Notes:
 *      - The function uses a brace pattern and a range of numbers to create multiple directories.
 *      - The 'mode' parameter specifies the file permission mode for the directories.
 *      - The 'real_path' parameter indicates the path where the directories will be created.
 *      - The 'find_dirname' parameter is the index of the directory name in the 'real_path' string.
 *      - The 'start' and 'end' parameters define the range of numbers for creating directories.
 *      - The function creates directories in parallel using multiple threads.
 *      - The function returns 0 if the range of numbers is invalid or if an error occurs during directory creation.
 */
int build_brace_dir(mode_t mode, char *real_path, int find_dirname, int start, int end) {
    
    char dir_name[MAX];
    
    // brc_pt_cnt : is the number of directories to create from the brace pattern.
    int brc_pt_cnt;
    if ((brc_pt_cnt = end - start + 1) <= 0) {
        fprintf(stderr, "Invalid dirname{%d..%d}\n", start, end);
        return 0;
    }
    
    pthread_t *thread_ids;
    ThreadArgs *thread_args;
    thread_ids = (pthread_t *) malloc(sizeof(pthread_t) * brc_pt_cnt);
    thread_args = (ThreadArgs *) malloc(sizeof(ThreadArgs) * brc_pt_cnt);
    
    // dirfd: is used to verify the path of the directory
    //          to be created and to create the directory.
    int dirfd;
    if ((dirfd = open(real_path, O_RDONLY)) == -1) {
        fprintf(stderr, "open funct failed\n");
        return 0;
    }
    
    // res: is used for exception.
    int res;
    int j;
    // Get a directory name from real_path
    strcpy(dir_name, strtok(real_path + find_dirname, "{"));
    for (j = 0; j < brc_pt_cnt; j++) {
        /**
         *  Make args for thread.
         */
        thread_args[j].dirfd = dirfd;
        sprintf(thread_args[j].dir_name, "%s%d", dir_name, start + j);
        thread_args[j].mode = mode;
        
        res = pthread_create(&thread_ids[j], NULL, pthread_mkdir_func, (void *)&thread_args[j]);
        if (res != 0) {
            perror("Failed to create thread");
            return 0;
        }
    }
    
    res = 0;
    for (j = 0; j < brc_pt_cnt; j++) {
        res = pthread_join(thread_ids[j], NULL);
        if (res != 0) {
            perror("Failed to join thread");
            return 0;
        } else {
            if (v_flag)
                printf("%s/%s\n", real_path, thread_args[j].dir_name);
            FILE* dirFile = fopen("log.txt", "a+");
            if (dirFile == NULL)
                perror("can't open log.txt");
            else
                print_log(dirFile, thread_args[j].dir_name);
            
        }
    }
    
    close(dirfd);
    free(thread_ids);
    free(thread_args);
    
    return 1;
}

int find_brace_pattern(char *target, int *start, int *end) {
    
    char temp[MAX];
    // Parsing from "abc{1..3}" for get baseName, start, end
    if (sscanf(target, "%1024[^{]{%d..%d}", temp, start, end) == 3 && target[strlen(target) - 1] == '}')
        return 1;
    else
        return 0;
}

void *pthread_mkdir_func(void *arg_) {
    
    ThreadArgs *arg = (ThreadArgs *) arg_;
    
    int result = syscall(SYS_mkdirat, arg->dirfd, arg->dir_name, arg->mode);
    if (result < 0) {
        perror("mkdir");
        pthread_exit((void *)1);
    }
    
    pthread_exit(NULL);
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

int isAbsolutePath(const char *path) {
    return (path[0] == '/');
}

void print_log(FILE *dirFile, char *dir_name) {
    
    char line[MAX];
    int nameExists = 0;
    while (fgets(line, MAX, dirFile) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line + 2, dir_name) == 0) {
            nameExists = 1;
            break;
        }
    }
    fclose(dirFile);

    if (!nameExists) {
        dirFile = fopen("log.txt", "a");
        if (dirFile == NULL) {
            perror("can't open log.txt");
            return;
        }
        fprintf(dirFile, "d %s\n", dir_name);
        fclose(dirFile);
    }
    
    return;
}

