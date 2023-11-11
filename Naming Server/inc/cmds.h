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

#define IP_SIZE     16
#define MAX_PATH_SIZE 4096
#define MAX_NAME_SIZE   1024
#define MAX_ENTRIES     1024
#define str     char*
#define str_arr char**
#define PARAMS(x) &x, sizeof(x), 0
#define SPARAM(x) x, strlen(x), 0
#define NM_CLIENT_PORT  5050 // clients send connecting request to 5050 port
#define NM_SERVER_PORT  6060 // SS send connecting request on 6060 port 
#define MAX_INPUT_SIZE 5000 
#define MAX_WORDS   512



typedef struct entry
{
    int id;
    char ip[IP_SIZE];
    int cport;
    int nmport;
    int entries;
    char paths[MAX_ENTRIES][MAX_PATH_SIZE];
}entry;
entry EMPTY_ENTRY;
typedef struct command
{
    int client;
    int argc;
    char cmd[MAX_INPUT_SIZE];
    char argv[MAX_WORDS][MAX_INPUT_SIZE];
}command;
// readFIle(c.argv[argc - 1]) write ./a.txt "hstring"  ... write "string" /.txt
// opern ... files
command parser(int socket)
{
    char input[MAX_INPUT_SIZE];
    memset(input, 0 , sizeof(input));
    fgets(input, MAX_INPUT_SIZE, stdin);
    command cmd;
    cmd.client = socket;
    char del[] = " \t\n\v\r\f";
    int n = strlen(input), i = 0;
    strcpy(cmd.cmd, input);
    cmd.cmd[n] = '\0';
    str token = strtok(input, del);
    while(token)
    {
        strcpy(cmd.argv[i++], token);
        token = strtok(NULL, del);
    }
    cmd.argc = i;
    return cmd;
}
// connection(&network_socket, &server_address, &connection_status)
int connection(int* socket, struct sockaddr_in * addr, int *stat)
{
    *stat = connect(*socket, (struct sockaddr*) addr, sizeof(*addr));
    if(*stat == -1) return 1;
    return 0;
}

//         msgsend(&network_socket, client_request);
int msgsend(int * network_socket, char * client_request) {
    ssize_t bytes_sent = send( * network_socket, client_request, sizeof(client_request), 0);
    return bytes_sent == -1 ? 1:0;
}

int recvreply(int * network_socket, char *reply)
{
    ssize_t bytes_recved = recv(*network_socket, reply, sizeof(reply), 0);
    return bytes_recved == -1 ? 1:0;
}
void recv_command(int socket, command *c)
{
    int client, argc;
    char cmd[MAX_INPUT_SIZE];
    recv(socket, PARAMS(client));
    recv(socket, PARAMS(argc));
    memset(cmd, 0, sizeof(cmd));
    int x = recv(socket, cmd, sizeof(cmd), 0);
    c->client = client; c->argc = argc;
    memset(c->cmd, 0, sizeof(c->cmd));
    strcpy(c->cmd, cmd);
    send(socket, PARAMS(x));
    for(int i=0; i<c->argc; i++)
    {
        char temp[MAX_INPUT_SIZE];
        memset(temp, 0, sizeof(temp));
        memset(c->argv[i], 0, sizeof(c->argv[i]));
        int res = recv(socket, temp, sizeof(temp), 0);
        strcpy(c->argv[i], temp);
        send(socket, PARAMS(res));
        c->argv[i][strlen(c->argv[i])] = '\0';
    }
}

void recv_entry(int socket, entry *e)
{
    int id, cport, nmport, entries;
    char IP[IP_SIZE];
    recv(socket, PARAMS(id));
    recv(socket, PARAMS(cport));
    recv(socket, PARAMS(nmport));
    recv(socket, PARAMS(entries));
    memset(e->ip, 0, sizeof(e->ip));
    int x = recv(socket, IP, sizeof(IP), 0);
    e->id = id; e->cport = cport; e->nmport = nmport; e->entries = entries;
    strcpy(e->ip, IP);
    send(socket, PARAMS(x));
    for(int i=0; i<entries; i++)
    {
        // send an arbitrary number to block
        char temp[MAX_PATH_SIZE];
        memset(temp, 0, sizeof(temp));
        memset(e->paths[i], 0, sizeof(e->paths[i]));
        int res = recv(socket, temp, sizeof(temp), 0);
        strcpy(e->paths[i], temp);
        send(socket, PARAMS(res));
        e->paths[i][strlen(e->paths[i])] = '\0';
    }
}
void send_command(int socket, command * c)
{
    int client = c->client, argc = c->argc;
    char cmd[MAX_INPUT_SIZE];
    strcpy(cmd, c->cmd);
    send(socket, PARAMS(client));
    send(socket, PARAMS(argc));
    int x = send(socket, SPARAM(cmd));
    recv(socket, PARAMS(x)); // ack
    for(int i=0; i<c->argc; i++)
    {
        int res = send(socket, SPARAM(c->argv[i]));
        recv(socket, PARAMS(res)); // ack
    }
}
void send_entry(int socket, entry *e)
{
    int id = e->id, cport = e->cport, nmport = e->nmport, entries = e->entries;
    char IP[IP_SIZE];
    strcpy(IP, e->ip);
    send(socket, PARAMS(id));
    send(socket, PARAMS(cport));
    send(socket, PARAMS(nmport));
    send(socket, PARAMS(entries));
    int x = send(socket, SPARAM(IP));
    recv(socket, PARAMS(x)); // ack
    for(int i=0; i<entries; i++)
    {
        int res = send(socket, SPARAM(e->paths[i]));
        recv(socket, PARAMS(res)); // ack
    }
}

void empty_entry(entry *e)
{
    e->id = -1;
    e->cport = -1;
    e->nmport = -1;
    e->ip[0] = '\0';
    e->entries = -1;
    for (int i = 0; i < MAX_ENTRIES; i++) {
        e->paths[i][0] = '\0';
    }
}

sem_t clientLock;

int writeToFile(const char *filename, const char *content) {
    // sem_wait(&clientLock);
    FILE *file = fopen(filename, "w");
    
    if (file == NULL) {
        sem_post(&clientLock);
        return 1; // Return error code 1 to indicate file open error
    }
    
    size_t content_length = strlen(content);
    size_t bytes_written = fwrite(content, 1, content_length, file);
    
    if (bytes_written != content_length) {
        fclose(file);
        sem_post(&clientLock);
        return 2; // Return error code 2 to indicate write error
    }
    
    fclose(file);
    sem_post(&clientLock);
    return 0; // Return 0 to indicate success
}

int appendToFile(const char *filename, const char *content) {
    // sem_wait(&clientLock);
    FILE *file = fopen(filename, "a");
    
    if (file == NULL) {
        sem_post(&clientLock);
        return 1; // Return error code 1 to indicate file open error
    }
    
    size_t content_length = strlen(content);
    size_t bytes_written = fwrite(content, 1, content_length, file);
    
    if (bytes_written != content_length) {
        fclose(file);
        sem_post(&clientLock);
        return 2; // Return error code 2 to indicate write error
    }
    
    fclose(file);
    sem_post(&clientLock);
    return 0; // Return 0 to indicate success
}

int deleteFile(const char *filename) {
    // sem_wait(&clientLock);
    if (remove(filename) == 0) {
        sem_post(&clientLock);
        return 0; // Return 0 to indicate success
    } else {
        sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }
}

int moveFile(const char *source, const char *destination) {
    // sem_wait(&clientLock);
    if (rename(source, destination) == 0) {
        sem_post(&clientLock);
        return 0; // Return 0 to indicate success
    } else {
        sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }
}

int getFileInfo(const char *filename, char *infoBuffer, size_t bufferSize) {
    // sem_wait(&clientLock);
    struct stat file_info;
    if (stat(filename, &file_info) != 0) {
        sem_post(&clientLock);
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
        sem_post(&clientLock);
        return 0; // Return 0 to indicate success
    } 
    else {
        sem_post(&clientLock);
        return 2; // Return 2 to indicate a buffer overflow error
    }
}

int readFile(const char *filename, char **buffer, size_t *size) {
    // sem_wait(&clientLock);
    FILE *file = fopen(filename, "r");
    
    if (file == NULL) {
        sem_post(&clientLock);
        return 1; // Return error code 1 to indicate file open error
    }
    
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *buffer = (char *)malloc(*size);
    
    if (*buffer == NULL) {
        fclose(file);
        sem_post(&clientLock);
        return 2; // Return error code 2 to indicate memory allocation error
    }

    size_t bytesRead = fread(*buffer, 1, *size, file);
    
    if (bytesRead != *size) {
        free(*buffer);
        fclose(file);
        sem_post(&clientLock);
        return 3; // Return error code 3 to indicate read error
    }
    
    fclose(file);
    sem_post(&clientLock);
    return 0; // Return 0 to indicate success
}

int makeDirectory(const char *path) {
    // sem_wait(&clientLock);
    if (mkdir(path, 0777) == 0) {
        sem_post(&clientLock);
        return 0; // Return 0 to indicate success
    } 
    else {
        sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }
}

int deleteDirectory(const char *path) {
    // sem_wait(&clientLock);
    struct dirent *entry;
    DIR *dp = opendir(path);

    if (dp == NULL) {
        sem_post(&clientLock);
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
                    sem_post(&clientLock);
                    return 1; // Return 1 to indicate an error
                }
            }
        }
    }

    closedir(dp);

    if (rmdir(path) == 0) {
        sem_post(&clientLock);
        return 0; // Return 0 to indicate success
    } else {
        sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }
}

int copyFile(const char *source, const char *destination) {
    // sem_wait(&clientLock);
    FILE *src_file = fopen(source, "rb");
    if (src_file == NULL) {
        sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }

    FILE *dest_file = fopen(destination, "wb");
    if (dest_file == NULL) {
        fclose(src_file);
        sem_post(&clientLock);
        return 1; // Return 1 to indicate an error
    }

    char buffer[1024];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        size_t bytes_written = fwrite(buffer, 1, bytes_read, dest_file);
        if (bytes_written != bytes_read) {
            fclose(src_file);
            fclose(dest_file);
            sem_post(&clientLock);
            return 1; // Return 1 to indicate an error
        }
    }

    fclose(src_file);
    fclose(dest_file);
    sem_post(&clientLock);
    return 0; // Return 0 to indicate success
}

int copyDirectory(const char *source, const char *destination) {
    // sem_wait(&clientLock);
    DIR *dir = opendir(source);
    if (dir == NULL) {
        sem_post(&clientLock);
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
                    sem_post(&clientLock);
                    return 1; // Return 1 to indicate an error
                }
            } else {
                if (copyFile(sourcePath, destinationPath) != 0) {
                    closedir(dir);
                    sem_post(&clientLock);
                    return 1; // Return 1 to indicate an error
                }
            }
        }
    }

    closedir(dir);
    sem_post(&clientLock);
    return 0; // Return 0 to indicate success
}



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

