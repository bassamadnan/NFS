/*
Shortcuts of commands used
*/
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h> // for group
#include <pwd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>  
#include <sys/utsname.h>
#include <sys/wait.h> // wuncrate
#include <time.h>
#include <time.h>
#include <unistd.h>
#include<stdio.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include "commands.h"
#include "defs.h"
#include "network.h"
#include "file_operations.h"

///////////////////////////////////////////////////////////////



//sem_t clientLock;




/////////////////////////////////////////////////////
//									COMMAND EXEC									 //
/////////////////////////////////////////////////////

/*
    create -f path (done)
    create -d path (done)

    delete -f path (done)
    delete -d path (done)

    copy -f src_path dest_path (done)
    copy -d src_path dest_path (done)

    move -f src_path dest_path (done)
    move -d src_path dest_path (done)

    write content path (done)
    read path (done)
    getinfo path (done)
*/




int executeCmd(command * cmd, int socket){
    
    if (cmd->argc < 2) {
        printf("Insufficient arguments.\n");
        return 1; 
    }

    char *subcmd = cmd->argv[0];

    if (strcmp(subcmd, "create") == 0) 
    {
        if (cmd->argc < 3) 
        {
            printf("Insufficient arguments for create command.\n");
            return 1;
        }

        char *type = cmd->argv[1];
        char *path = cmd->argv[2];

        if (strcmp(type, "-f") == 0) 
        {
            return writeToFile(path, "");
        } 
        else if (strcmp(type, "-d") == 0) 
        {
            return makeDirectory(path);
        } 
        else 
        {
            printf("Invalid create subcommand.\n");
            return 1;
        }
    } 
    else if (strcmp(subcmd, "delete") == 0) 
    {
        if (cmd->argc < 3) 
        {
            printf("Insufficient arguments for delete command.\n");
            return 1; 
        }

        char *type = cmd->argv[1];
        char *path = cmd->argv[2];

        if (strcmp(type, "-f") == 0) 
        {
            return deleteFile(path);
        } 
        else if (strcmp(type, "-d") == 0) 
        {
            return deleteDirectory(path);
        } 
        else 
        {
            printf("Invalid delete subcommand.\n");
            return 1; 
        }
    } 
    else if (strcmp(subcmd, "copy") == 0) 
    {
        if (cmd->argc < 4) 
        {
            printf("Insufficient arguments for copy command.\n");
            return 1;
        }

        char *type = cmd->argv[1];
        char *src_path = cmd->argv[2];
        char *dest_path = cmd->argv[3];

        if (strcmp(type, "-f") == 0) 
        {
            return copyFile(src_path, dest_path);
        } 
        else if (strcmp(type, "-d") == 0) 
        {
            return copyDirectory(src_path, dest_path);
        } 
        else 
        {
            printf("Invalid copy subcommand.\n");
            return 1;
        }
    } 
    else if (strcmp(subcmd, "move") == 0) 
    {
        if (cmd->argc < 4) 
        {
            printf("Insufficient arguments for move command.\n");
            return 1;
        }

        char *type = cmd->argv[1];
        char *src_path = cmd->argv[2];
        char *dest_path = cmd->argv[3];

        if (strcmp(type, "-f") == 0) 
        {
            return moveFile(src_path, dest_path);
        } 
        else if (strcmp(type, "-d") == 0) 
        {
            return moveFile(src_path, dest_path);
        } 
        else 
        {
            printf("Invalid move subcommand.\n");
            return 1; 
        }
    } 
    else if (strcmp(subcmd, "write") == 0) 
    {
        if (cmd->argc < 3) 
        {
            printf("Insufficient arguments for write command.\n");
            return 1; 
        }

        char content[MAX_INPUT_SIZE] = "";

        // Construct content from the arguments (except the last one, which is the path)
        for (int i = 1; i < cmd->argc - 1; i++) 
        {
            strncat(content, cmd->argv[i], sizeof(content) - strlen(content) - 1);
            if (i < cmd->argc - 2) 
            {
                strncat(content, " ", sizeof(content) - strlen(content) - 1);
            }
        }

        char *path = cmd->argv[cmd->argc - 1]; // Last argument is the path

        return writeToFile(path, content);
    }
    else if (strcmp(subcmd, "read") == 0) 
    {
        if (cmd->argc < 2) {
            printf("Insufficient arguments for read command.\n");
            return 1; 
        }

        char *path = cmd->argv[1];
        char *buffer = NULL;
        size_t size = 0;
        int result = readFile(path, &buffer, &size);
        printf("result stat %d\n", result);
        if (result == 0) {
            int x = send(socket, buffer, size, 0);
            printf("sent %d\n", x);
            free(buffer);
        }

        return result;
    }
    else if (strcmp(subcmd, "getinfo") == 0) {
        if (cmd->argc < 2) {
            printf("Insufficient arguments for getinfo command.\n");
            return 1; 
        }

        char *path = cmd->argv[1];
        char infoBuffer[4096]; 

        int result = getFileInfo(path, infoBuffer, sizeof(infoBuffer));
        send(cmd->client, infoBuffer, strlen(infoBuffer), 0);
        return result;
    }
    else 
    {
        printf("Invalid command.\n");
        return 1; 
    }

    return 0;
}

