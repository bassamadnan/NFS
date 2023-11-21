#include "../inc/command_execution.h"
#include <dirent.h>
#include "../inc/hash.h"
typedef struct HashMap {
    struct KeyValue* table[MAP_SIZE];
} map_t;

int is_directory(const str path) {
    DIR *directory = opendir(path);
    if (directory) {
        closedir(directory);
        return 1;
    }
    return 0;
}


void copyDir(int socket, str path)
{
    command *c = malloc(sizeof(command));
    create_command(c, path); // create -d path
    int operation = MKDIR;
    send(socket, PARAMS(operation));
    printf("Sending %s\n", c->argv[2]);
    send_command(socket, c);
    printf("Sent %s\n", c->argv[2]);
    free(c);
}

void send_directory(const str dir_path, int socket) {
   
    DIR *dir = opendir(dir_path);
    copyDir(socket,(str) dir_path);
    if (!dir) {
        fprintf(stderr, "Failed to open directory: %s\n", dir_path);
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;  // Skip current directory and parent directory entries.
        }

        char full_path[MAX_PATH_SIZE];
        memset(full_path, 0, sizeof(full_path));
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        printf("full_path : %s\n", full_path);
        if (strlen(entry->d_name) && is_directory(full_path)) {
            // create this directory
            send_directory(full_path, socket);  // Recursive call for subdirectories.
        } else {
            int operation = MKFIL;
            send(socket, PARAMS(operation));
            command *c = malloc(sizeof(command));
            create_command(c, full_path);
            send_command(socket, c);
            send_file(socket, full_path);
            free(c);
        }
    }

    closedir(dir);
}
void send_directory_backup(const str dir_path, int socket, map_t *sent_paths) {
    if(find(sent_paths, dir_path)) return;
    DIR *dir = opendir(dir_path);
    copyDir(socket,(str) dir_path);
    printf("reacher \n");
    insert(sent_paths, dir_path, 1);
    if (!dir) {
        fprintf(stderr, "Failed to open directory: %s\n", dir_path);
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;  // Skip current directory and parent directory entries.
        }

        char full_path[MAX_PATH_SIZE];
        memset(full_path, 0, sizeof(full_path));
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        printf("full_path : %s\n", full_path);
        if (strlen(entry->d_name) && is_directory(full_path)) {
            // create this directory
            send_directory_backup(full_path, socket, sent_paths);  // Recursive call for subdirectories.
        } else {
            int operation = MKFIL;
            send(socket, PARAMS(operation));
            command *c = malloc(sizeof(command));
            create_command(c, full_path);
            send_command(socket, c);
            send_file(socket, full_path);
            insert(sent_paths, full_path, 1);
            free(c);
        }
    }

    closedir(dir);
}

int executeCmd(command * cmd, int socket, int PERMISSIONS){
    
    if (cmd->argc < 2) {
        // printf("Insufficient arguments.\n");
        return -1; 
    }

    char *subcmd = cmd->argv[0];

    if (strcmp(subcmd, "create") == 0) 
    {
        if (cmd->argc < 3) 
        {
            // printf("Insufficient arguments for create command.\n");
            return -1;
        }

        if(!(PERMISSIONS & CRT))
        {
            return -4;
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
            // printf("Invalid create subcommand.\n");
            return -2;
        }
    } 
    else if (strcmp(subcmd, "delete") == 0) 
    {
        if (cmd->argc < 3) 
        {
            // printf("Insufficient arguments for delete command.\n");
            return -1; 
        }
        if(!(PERMISSIONS & DLT))
        {
            return -4;
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
            // printf("Invalid delete subcommand.\n");
            return -2; 
        }
    } 
    else if (strcmp(subcmd, "copy") == 0) // 200 , 201-> f , 202 -> copyd
    {
        if (cmd->argc < 4) 
        {
            // printf("Insufficient arguments for copy command.\n");
            return -1;
        }
        
        if(!(PERMISSIONS & CPY))
        {
            return -4;
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
            // printf("Invalid copy subcommand.\n");
            return -2;
        }
    } 
    else if (strcmp(subcmd, "move") == 0) 
    {
        if (cmd->argc < 4) 
        {
            // printf("Insufficient arguments for move command.\n");
            return -1;
        }

        if(!(PERMISSIONS & MOV))
        {
            return -4;
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
            // printf("Invalid move subcommand.\n");
            return -2; 
        }
    } 
    else if (strcmp(subcmd, "write") == 0) 
    {
        if (cmd->argc < 3) 
        {
            // printf("Insufficient arguments for write command.\n");
            return -1; 
        }

        if(!(PERMISSIONS & WRT))
        {
            return -4;
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
            // printf("Insufficient arguments for read command.\n");
            return -1; 
        }

        if (!(PERMISSIONS && RED))
        {
            return -4;
        }

        char *path = cmd->argv[1];
        char *buffer = NULL;
        size_t size = 0;
        size = size;
        int result = readFile(path, socket);
        printf("result stat %d\n", result);
        if (result == 0) {
            // int x = send(socket, buffer, size, 0);
            // printf("sent %d\n", x);
            free(buffer);
        }
        return result;
    }
    else if (strcmp(subcmd, "getinfo") == 0) {
        if (cmd->argc < 2) {
            // printf("Insufficient arguments for getinfo command.\n");
            return -1; 
        }

        if(!(PERMISSIONS & INF))
        {
            return -4;
        }

        char *path = cmd->argv[1];
        char infoBuffer[500]; 

        int result = getFileInfo(path, infoBuffer, sizeof(infoBuffer));
        printf("Info : %s\n", infoBuffer);
        send(socket, infoBuffer, strlen(infoBuffer), 0);
        return result;
    }
    else if(strcmp(subcmd, "append") == 0){
        if (cmd->argc < 3) 
        {
            // printf("Insufficient arguments for append command.\n");
            return -1; 
        }

        if(!(PERMISSIONS & WRT))
        {
            return -4;
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

        return appendToFile(path, content);
    }
    else 
    {
        // printf("Invalid command.\n");
        return -3; 
    }

    return 0;
}