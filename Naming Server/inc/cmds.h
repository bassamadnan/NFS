#ifndef CMDS
#define CMDS
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


#define IP_SIZE     16
#define MAX_PATH_SIZE 4096
#define MAX_NAME_SIZE   1024
#define MAX_ENTRIES     1024
#define str     char*
#define str_arr char**
#define PARAMS(x) &x, sizeof(x), 0
#define NM_CLIENT_PORT  5050 // clients send connecting request to 5050 port
#define NM_SERVER_PORT  6060 // SS send connecting request on 6060 port 
#define MAX_INPUT_SIZE 5000 
#define MAX_WORDS   512

typedef struct entry
{
    int id;
    char ip[IP_SIZE];
    int nmport;
    int cport;
    int entries;
    char paths[MAX_ENTRIES][MAX_PATH_SIZE];
    char init_path[MAX_PATH_SIZE];
}entry;

typedef struct command
{
    int argc;
    char cmd[MAX_INPUT_SIZE];
    char argv[MAX_WORDS][MAX_INPUT_SIZE];
}command;

void parser(command *cmd)
{
    char input[MAX_INPUT_SIZE];
    fgets(input, MAX_INPUT_SIZE, stdin);
    char del[] = " \t\n\v\r\f";
    int n = strlen(input), i = 0;
    strcpy(cmd->cmd, input);
    cmd->cmd[n] = '\0';
    str token = strtok(input, del);
    while(token)
    {
        strcpy(cmd->argv[i++], token);
        token = strtok(NULL, del);
    }
    cmd->argc = i;
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

#endif