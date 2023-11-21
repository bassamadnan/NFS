#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h> 
#include <semaphore.h>

#define COLOR_RED     "\x1b[31m"
#define RESET   "\x1b[0m"

struct index2D 
{
    int row;
    int col;
};

struct index2D inferCode(const char* cmd, int code, int fileFlag);
char* errorMessage[][4];

#endif 