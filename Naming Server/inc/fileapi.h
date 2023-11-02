#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h> 


int writeToFile(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    
    if (file == NULL) {
        return 1; // Return error code 1 to indicate file open error
    }
    
    size_t content_length = strlen(content);
    size_t bytes_written = fwrite(content, 1, content_length, file);
    
    if (bytes_written != content_length) {
        fclose(file);
        return 2; // Return error code 2 to indicate write error
    }
    
    fclose(file);
    return 0; // Return 0 to indicate success
}

int appendToFile(const char *filename, const char *content) {
    FILE *file = fopen(filename, "a");
    
    if (file == NULL) {
        return 1; // Return error code 1 to indicate file open error
    }
    
    size_t content_length = strlen(content);
    size_t bytes_written = fwrite(content, 1, content_length, file);
    
    if (bytes_written != content_length) {
        fclose(file);
        return 2; // Return error code 2 to indicate write error
    }
    
    fclose(file);
    return 0; // Return 0 to indicate success
}

int deleteFile(const char *filename) {
    if (remove(filename) == 0) {
        return 0; // Return 0 to indicate success
    } else {
        return 1; // Return 1 to indicate an error
    }
}

int moveFile(const char *source, const char *destination) {
    if (rename(source, destination) == 0) {
        return 0; // Return 0 to indicate success
    } else {
        return 1; // Return 1 to indicate an error
    }
}

int getFileInfo(const char *filename) {
    struct stat file_info;
    
    if (stat(filename, &file_info) == 0) {
        printf("File: %s\n", filename);
        printf("Size: %lld bytes\n", (long long)file_info.st_size);
        printf("Mode: %o\n", file_info.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
        printf("Owner User ID: %d\n", file_info.st_uid);
        printf("Owner Group ID: %d\n", file_info.st_gid);
        printf("Device ID: %ld\n", (long)file_info.st_dev);
        printf("Inode: %ld\n", (long)file_info.st_ino);
        printf("Number of Hard Links: %ld\n", (long)file_info.st_nlink);
        printf("Last Access Time: %ld\n", (long)file_info.st_atime);
        printf("Last Modification Time: %ld\n", (long)file_info.st_mtime);
        printf("Last Status Change Time: %ld\n", (long)file_info.st_ctime);
        return 0; // Return 0 to indicate success
    } else {
        return 1; // Return 1 to indicate an error
    }
}

int readFile(const char *filename, char **buffer, size_t *size) {
    FILE *file = fopen(filename, "r");
    
    if (file == NULL) {
        return 1; // Return error code 1 to indicate file open error
    }
    
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *buffer = (char *)malloc(*size);
    
    if (*buffer == NULL) {
        fclose(file);
        return 2; // Return error code 2 to indicate memory allocation error
    }

    size_t bytesRead = fread(*buffer, 1, *size, file);
    
    if (bytesRead != *size) {
        free(*buffer);
        fclose(file);
        return 3; // Return error code 3 to indicate read error
    }
    
    fclose(file);
    return 0; // Return 0 to indicate success
}

int makeDirectory(const char *path) {
    if (mkdir(path, 0777) == 0) {
        return 0; // Return 0 to indicate success
    } else {
        return 1; // Return 1 to indicate an error
    }
}

int deleteDirectory(const char *path) {
    struct dirent *entry;
    DIR *dp = opendir(path);

    if (dp == NULL) {
        return 1; // Return 1 to indicate an error
    }

    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char entryPath[PATH_MAX];
            snprintf(entryPath, sizeof(entryPath), "%s/%s", path, entry->d_name);

            if (entry->d_type == DT_DIR) {
                deleteDirectory(entryPath); 
            } else {
                if (remove(entryPath) != 0) {
                    perror("Error deleting file");
                    closedir(dp);
                    return 1; // Return 1 to indicate an error
                }
            }
        }
    }

    closedir(dp);

    if (rmdir(path) == 0) {
        return 0; // Return 0 to indicate success
    } else {
        return 1; // Return 1 to indicate an error
    }
}

int copyFile(const char *source, const char *destination) {
    FILE *src_file = fopen(source, "rb");
    if (src_file == NULL) {
        return 1; // Return 1 to indicate an error
    }

    FILE *dest_file = fopen(destination, "wb");
    if (dest_file == NULL) {
        fclose(src_file);
        return 1; // Return 1 to indicate an error
    }

    char buffer[1024];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        size_t bytes_written = fwrite(buffer, 1, bytes_read, dest_file);
        if (bytes_written != bytes_read) {
            fclose(src_file);
            fclose(dest_file);
            return 1; // Return 1 to indicate an error
        }
    }

    fclose(src_file);
    fclose(dest_file);
    return 0; // Return 0 to indicate success
}

int copyDirectory(const char *source, const char *destination) {
    DIR *dir = opendir(source);
    if (dir == NULL) {
        return 1; // Return 1 to indicate an error
    }

    mkdir(destination, 0777);

    struct dirent *entry;

    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char sourcePath[PATH_MAX];
            char destinationPath[PATH_MAX];
            snprintf(sourcePath, sizeof(sourcePath), "%s/%s", source, entry->d_name);
            snprintf(destinationPath, sizeof(destinationPath), "%s/%s", destination, entry->d_name);

            if (entry->d_type == DT_DIR) {
                if (copyDirectory(sourcePath, destinationPath) != 0) {
                    closedir(dir);
                    return 1; // Return 1 to indicate an error
                }
            } else {
                if (copyFile(sourcePath, destinationPath) != 0) {
                    closedir(dir);
                    return 1; // Return 1 to indicate an error
                }
            }
        }
    }

    closedir(dir);
    return 0; // Return 0 to indicate success
}