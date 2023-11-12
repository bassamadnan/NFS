// file_operations.h
#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <semaphore.h>

#define IP_SIZE 16
#define MAX_PATH_SIZE 4096
#define MAX_ENTRIES 1024

typedef struct entry {
    int id;
    char ip[IP_SIZE];
    int cport;
    int nmport;
    int entries;
    char paths[MAX_ENTRIES][MAX_PATH_SIZE];
} entry;

extern entry EMPTY_ENTRY;
extern sem_t clientLock;

int writeToFile(const char *filename, const char *content);
int appendToFile(const char *filename, const char *content);
int deleteFile(const char *filename);
int moveFile(const char *source, const char *destination);
int getFileInfo(const char *filename, char *infoBuffer, size_t bufferSize);
int readFile(const char *filename, char **buffer, size_t *size);
int makeDirectory(const char *path);
int deleteDirectory(const char *path);
int copyFile(const char *source, const char *destination);
int copyDirectory(const char *source, const char *destination);

void empty_entry(entry *e);

#endif // FILE_OPERATIONS_H

