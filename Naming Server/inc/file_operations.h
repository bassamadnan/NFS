// file_operations.h
#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include "defs.h"
#include <semaphore.h>

#include "entry.h"

// sem_t clientLock; //does this need to be extern check that

int writeToFile(const char *filename, const char *content);
int appendToFile(const char *filename, const char *content);
int deleteFile(const char *filename);
int moveFile(const char *source, const char *destination);
int getFileInfo(const char *filename, char *infoBuffer, size_t bufferSize);
int readFile(const char *filename, int client_socket);
int makeDirectory(const char *path);
int deleteDirectory(const char *path);
int copyFile(const char *source, const char *destination);
int copyDirectory(const char *source, const char *destination);

void empty_entry(entry *e);

#endif // FILE_OPERATIONS_H

