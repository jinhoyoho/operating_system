//
//  find_.h
//  OS_find
//
//  Created by 최정흠 on 2023/05/27.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

void find_(int argc, char *argv[]);
void find_files(const char *path);
void name_recur(char *name, char *target);
void type_recur(char *name, char *mode);
int isAbsolutePath_(const char *path);

void find_(int argc, char *argv[]) {
    
    char current_path[1024];
    char real_path[1024];
    char *temp;
    int subdir_flag;
    int i;
    
    if (argc == 1) {
        getcwd(current_path, sizeof(current_path));
        find_files(current_path);
    }
    
    if (!isAbsolutePath(argv[1])) { // Relative path
        /**
         *   Check if there is a subdirectory
         */
        subdir_flag = 0;
        for (i = 0; argv[1][i] != '\0'; i++)
            if (argv[1][i] == '/')
                subdir_flag = 1;
        
        if (strncmp(argv[1], "./", 2) == 0)
            subdir_flag = 0;
        if (strncmp(argv[1], "../", 3) == 0)
            subdir_flag = 0;
        /**
         *  Convert relative path to absolute path using realpath()
         */
        if (!subdir_flag)
            realpath(argv[1], real_path);
        else {
            temp = realpath(".", NULL);
            strcpy(real_path, temp);
            for (i = 0; real_path[i] != '\0'; i++);
            real_path[i] = '/';
            real_path[i + 1] = '\0';
            strcat(real_path, argv[1]);
        }
    } else // Absolute path
        strcpy(real_path, argv[1]);
    
    if (argv[2] != NULL){
        if(strcmp(argv[2], "-name") == 0)
            name_recur(real_path, argv[3]);
        else if(strcmp(argv[2], "-type") == 0)
            type_recur(real_path, argv[3]);
    } else
        find_files(real_path);
    
    return;
}

/**
 *  Recursively finds files and directories in the given path.
 *
 *  Param:
 *      const char *path
 *              The path to search for files and directories.
 *
 *  Return:
 *      None.
 *
 *  Notes:
 *      - This function recursively explores the directory structure starting from the given path.
 *      - For each directory entry (file or directory) found, it prints the full path.
 *      - Subdirectories are explored by calling the function recursively.
 *      - If an error occurs while opening the directory, an error message is printed.
*/
void find_files(const char *path) {
    
    DIR *dir;
    struct dirent *entry;
    char full_path[1024];

    if (!(dir = opendir(path))) {
        perror("opendir");
        return;
    }

    // Search directory entries
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        // Print file or directory
        printf("%s\n", full_path);

        // Recursively search subdirectories
        if (entry->d_type == DT_DIR)
            find_files(full_path);
    }

    closedir(dir);
}

/**
 *  Recursively searches for files and directories matching the target path in the given directory.
 *
 *  Param:
 *      char *name
 *          The directory path to search.
 *      char *target
 *          The target path (file or directory) to find within the directory.
 *
 *  Return:
 *      None.
 *
 *  Notes:
 *      - This function recursively explores the directory structure starting from the given directory path.
 *      - For each directory entry, it checks if the entry matches the target path and prints the corresponding path.
 *      - If the target path is a directory, it recursively calls itself with the remaining target path.
 *      - If the target path is a file, it prints the path if it matches.
 *      - If an error occurs while opening the directory, an error message is printed.
 */
void name_recur(char *name, char *target) {
    
    DIR *dir;
    struct dirent *entry;
    char path[1024];
    // ptr : is pointer to '/' character in the target path
    char *ptr;
    
    if (!(dir = opendir(name))) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            
            strcpy(path, name);
            strcat(path, "/");
            strcat(path, entry->d_name);

            /**
             * If the target directory matches, print the path
             */
            if (target && strcmp(entry->d_name, target) == 0)
                printf("%s\n", path);
            
            /**
             * Find the '/' character in the target path
             */
            ptr = target ? strchr(target, '/') : NULL;
            
            /**
             * Recursively call name_recur with the remaining target path
             */
            if (ptr != NULL)
                name_recur(path, target + (ptr - target + 1));
        } else {
            /**
             * If the target file matches, print the path*
             */
            if (target && strcmp(entry->d_name, target) == 0)
                printf("%s/%s\n", name, entry->d_name);
        }
    }
    
    closedir(dir);
    
    return;
}

/**
 *  Recursively searches for files and directories based on the specified mode in the given directory.
 *
 *  Param:
 *      char *name
 *          The directory path to search.
 *      char *mode
 *          The mode indicating whether to search for directories "d" or files "f".
 *
 *  Return:
 *      None.
 *
 *  Note:
 *      - This function recursively explores the directory structure starting from the given directory path.
 *      - For each directory entry, it checks if the entry is a directory and matches the specified mode "d".
 *      If so, it prints the corresponding path.
 *      - If the specified mode is "d", it recursively calls itself with the current entry.
 *      - For non-directory entries, if the specified mode is "f", it prints the file path.
 *      - If an error occurs while opening the directory, an error message is printed.
 */
void type_recur(char *name, char *mode) {
    
    DIR *dir;
    struct dirent *entry;
    char path[1024];

    if (!(dir = opendir(name))) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            strcpy(path, name);
            strcat(path, "/");
            strcat(path, entry->d_name);
            if (strcmp(mode, "d") == 0)
                printf("%s\n", path);
            
            type_recur(path, mode);
        } else if(strcmp(mode, "f") == 0)
            printf("%s/%s\n", name, entry->d_name);
    }
    
    closedir(dir);
    
    return;
}

int isAbsolutePath_(const char *path) {
    return (path[0] == '/');
}
