// network.h
#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h>
#include "defs.h"
#include "commands.h"

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

int connection(int *socket, struct sockaddr_in *addr, int *stat);
int msgsend(int *network_socket, char *client_request);
int recvreply(int *network_socket, char *reply);
void recv_command(int socket, command *c);
void recv_entry(int socket, entry *e);
void send_command(int socket, command * c);
void send_entry(int socket, entry *e);

void empty_entry(entry *e);
#endif // NETWORK_H

