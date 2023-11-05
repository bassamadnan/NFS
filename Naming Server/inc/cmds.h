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

typedef struct command
{
    int client;
    int argc;
    char cmd[MAX_INPUT_SIZE];
    char argv[MAX_WORDS][MAX_INPUT_SIZE];
}command;

command parser(int socket)
{
    char input[MAX_INPUT_SIZE];
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
    if(e->id == -1) return; // HANDLE THIS!!
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