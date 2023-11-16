// file_operations.c
#include "../inc/file_operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>




int writeToFile(const char *filename, const char *content) {
    // // sem_wait(&clientLock);
    FILE *file = fopen(filename, "w");
    
    if (file == NULL) {
        // sem_post(&clientLock);
        return 1; // Return error code 1 to indicate file open error
    }
    
    size_t content_length = strlen(content);
    size_t bytes_written = fwrite(content, 1, content_length, file);
    
    if (bytes_written != content_length) {
        fclose(file);
        // sem_post(&clientLock);
        return 2; // Return error code 2 to indicate write error
    }
    
    fclose(file);
    // sem_post(&clientLock);
    return 0; // Return 0 to indicate success
}

int appendToFile(const char *filename, const char *content) {
    // // sem_wait(&clientLock);
    FILE *file = fopen(filename, "a");
    
    if (file == NULL) {
        // sem_post(&clientLock);
        return 1; // Return error code 1 to indicate file open error
    }
    
    size_t content_length = strlen(content);
    size_t bytes_written = fwrite(content, 1, content_length, file);
    
    if (bytes_written != content_length) {
        fclose(file);
        // sem_post(&clientLock);
        return 2; // Return error code 2 to indicate write error
    }
    
    fclose(file);
    // sem_post(&clientLock);
    return 0; // Return 0 to indicate success
}

int deleteFile(const char *filename) {
    // // sem_wait(&clientLock);
    if (remove(filename) == 0) {
        // sem_post(&clientLock);
        return 0; // Return 0 to indicate success
    } else {
        // sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }
}

int moveFile(const char *source, const char *destination) {
    // // sem_wait(&clientLock);
    if (rename(source, destination) == 0) {
        printf("reached rename\n");
        // sem_post(&clientLock);
        return 0; // Return 0 to indicate success
    } else {
        printf("couldnt reach rename\n");
        // sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }
}
int getFileInfo(const char *filename, char *infoBuffer, size_t bufferSize) {
    // // sem_wait(&clientLock);
    struct stat file_info;
    if (stat(filename, &file_info) != 0) {
        // sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }
    int result = snprintf(infoBuffer, bufferSize, "File: %s\nSize: %lld bytes\nMode: %o\nOwner User ID: %d\nOwner Group ID: %d\nDevice ID: %ld\nInode: %ld\nNumber of Hard Links: %ld\nLast Access Time: %ld\nLast Modification Time: %ld\nLast Status Change Time: %ld\n",
             filename,
             (long long)file_info.st_size,
             file_info.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO),
             file_info.st_uid,
             file_info.st_gid,
             (long)file_info.st_dev,
             (long)file_info.st_ino,
             (long)file_info.st_nlink,
             (long)file_info.st_atime,
             (long)file_info.st_mtime,
             (long)file_info.st_ctime);

    if (result >= 0 && result < bufferSize) {
        // sem_post(&clientLock);
        return 0; // Return 0 to indicate success
    } 
    else {
        // sem_post(&clientLock);
        return 2; // Return 2 to indicate a buffer overflow error
    }
}

int readFile(const char *filename, char **buffer, size_t *size) {
    // // sem_wait(&clientLock);
    FILE *file = fopen(filename, "r");
    
    if (file == NULL) {
        // sem_post(&clientLock);
        return 1; // Return error code 1 to indicate file open error
    }
    
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *buffer = (char *)malloc(*size);
    
    if (*buffer == NULL) {
        fclose(file);
        // sem_post(&clientLock);
        return 2; // Return error code 2 to indicate memory allocation error
    }

    size_t bytesRead = fread(*buffer, 1, *size, file);
    
    if (bytesRead != *size) {
        free(*buffer);
        fclose(file);
        // sem_post(&clientLock);
        return 3; // Return error code 3 to indicate read error
    }
    
    fclose(file);
    // sem_post(&clientLock);
    return 0; // Return 0 to indicate success
}

int makeDirectory(const char *path) {
    // // sem_wait(&clientLock);
    char * temp = calloc(MAX_PATH_SIZE, sizeof(char));
    strcpy(temp, "./");
    strcat(temp, path);
    if (mkdir(temp, 0777) == 0) {
        // sem_post(&clientLock);
        free(temp);
        return 0; // Return 0 to indicate success
    } 
    else {
        free(temp);
        // sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }
}

int deleteDirectory(const char *path) {
    // // sem_wait(&clientLock);
    struct dirent *entry;
    DIR *dp = opendir(path);

    if (dp == NULL) {
        // sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }

    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char entryPath[MAX_PATH_SIZE];
            snprintf(entryPath, sizeof(entryPath), "%s/%s", path, entry->d_name);

            if (entry->d_type == DT_DIR) {
                deleteDirectory(entryPath); 
            } else {
                if (remove(entryPath) != 0) {
                    perror("Error deleting file");
                    closedir(dp);
                    // sem_post(&clientLock);
                    return 1; // Return 1 to indicate an error
                }
            }
        }
    }

    closedir(dp);

    if (rmdir(path) == 0) {
        // sem_post(&clientLock);
        return 0; // Return 0 to indicate success
    } else {
        // sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }
}

int copyFile(const char *source, const char *destination) {
    // // sem_wait(&clientLock);
    FILE *src_file = fopen(source, "rb");
    if (src_file == NULL) {
        // sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }

    FILE *dest_file = fopen(destination, "wb");
    if (dest_file == NULL) {
        fclose(src_file);
        // sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }

    char buffer[1024];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        size_t bytes_written = fwrite(buffer, 1, bytes_read, dest_file);
        if (bytes_written != bytes_read) {
            fclose(src_file);
            fclose(dest_file);
            // sem_post(&clientLock);
            return 1; // Return 1 to indicate an error
        }
    }

    fclose(src_file);
    fclose(dest_file);
    // sem_post(&clientLock);
    return 0; // Return 0 to indicate success
}
int copyDirectory(const char *source, const char *destination) {
    // // sem_wait(&clientLock);
    DIR *dir = opendir(source);
    if (dir == NULL) {
        // sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }

    mkdir(destination, 0777);

    struct dirent *entry;

    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char sourcePath[MAX_PATH_SIZE];
            char destinationPath[MAX_PATH_SIZE];
            snprintf(sourcePath, sizeof(sourcePath), "%s/%s", source, entry->d_name);
            snprintf(destinationPath, sizeof(destinationPath), "%s/%s", destination, entry->d_name);

            if (entry->d_type == DT_DIR) {
                if (copyDirectory(sourcePath, destinationPath) != 0) {
                    closedir(dir);
                    // sem_post(&clientLock);
                    return 1; // Return 1 to indicate an error
                }
            } else {
                if (copyFile(sourcePath, destinationPath) != 0) {
                    closedir(dir);
                    // sem_post(&clientLock);
                    return 1; // Return 1 to indicate an error
                }
            }
        }
    }

    closedir(dir);
    // sem_post(&clientLock);
    return 0; // Return 0 to indicate success
}
