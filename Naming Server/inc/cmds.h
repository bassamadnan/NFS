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