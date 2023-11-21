
#include "../inc/error_codes.h"

char* errorMessage[][4] = {
    {COLOR_RED "Error 201: Failed to open the file" RESET, COLOR_RED "Error 202: Failed to write to the file" RESET, NULL},
    {COLOR_RED "Error 301: Failed to open the file" RESET, COLOR_RED "Error 302: Failed to append to the file" RESET, NULL},
    {COLOR_RED "Error 401: Failed to delete the file" RESET, NULL},
    {COLOR_RED "Error 501: Failed to move the file" RESET, NULL},
    {COLOR_RED "Error 601: Failed to get information about the file" RESET,COLOR_RED "Error 602: Informaton buffer smaller than expected" RESET, NULL},
    {COLOR_RED "Error 701: Failed to open the file" RESET, COLOR_RED "Error 702: Failed to allocate memory for reading" RESET, COLOR_RED "Error 703: Failed to read from the file" RESET, NULL},
    {COLOR_RED "Error 801: Failed to create the directory" RESET, NULL},
    {COLOR_RED "Error 901: Failed to open the directory" RESET,COLOR_RED "Error 902: Failed to delete a file within" RESET, COLOR_RED "Error 903: Failed to delete the directory" RESET, NULL},
    {COLOR_RED "Error 111: Failed to open source file" RESET, COLOR_RED "Error 112: Failed to open destination file" RESET, COLOR_RED "Error 113: Failed to copy the content" RESET, NULL},
    {COLOR_RED "Error 121: Failed to open source directory" RESET, COLOR_RED "Error 122: Failed to copy a sub directory" RESET,  COLOR_RED "Error 123: Failed to copy a file within" RESET, NULL},
    {COLOR_RED "Error 101: Permission denied" RESET, NULL},
    {COLOR_RED "Error 102: Invalid command" RESET, NULL},
    {COLOR_RED "Error 103: Invalid arguments for command" RESET, NULL},
    {COLOR_RED "Error 104: Insufficient arguments" RESET, NULL},
    {NULL}
};

struct index2D inferCode(const char* cmd, int code, int fileFlag)
{
    struct index2D ind;

    int length = 1;
    while (errorMessage[length][0] != NULL) {
        length++;
    }

    if (code < 0) 
    {
        ind.row = length-code-1;
        ind.col = 0;
        return ind;
    }

    ind.col = code-1;
    if (strcmp(cmd, "write") == 0 && fileFlag)
    {
        ind.row = 0;
    }
    else if (strcmp(cmd, "append"))
    {
        ind.row = 1;
    }
    else if (strcmp(cmd, "delete") == 0 && fileFlag)
    {
        ind.row = 2;
    }
    else if(strcmp(cmd, "move") == 0 && fileFlag)
    {
        ind.row = 3;
    }
    else if(strcmp(cmd, "getinfo") == 0)
    {
        ind.row = 4;
    }
    else if(strcmp(cmd, "read") == 0)
    {
        ind.row = 5;
    }
    else if(strcmp(cmd, "create") == 0 && !fileFlag)
    {
        ind.row = 6;
    }
    else if(strcmp(cmd, "delete") == 0 && !fileFlag)
    {
        ind.row = 7;
    }
    else if(strcmp(cmd, "copy") == 0 && fileFlag)
    {
        ind.row = 8;
    }
    else if(strcmp(cmd, "copy") == 0 && !fileFlag)
    {
        ind.row = 9;
    }
    return ind;
}

