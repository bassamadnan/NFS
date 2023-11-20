// network.h
#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h>
#include "defs.h"
#include "commands.h"
#include "entry.h"

typedef struct ACK
{
    int id;
    int code;  
    char message[MAX_MESSAGE_SIZE];  
} ACK;


int connection(int *socket, struct sockaddr_in *addr, int *stat);
int msgsend(int *network_socket, char *client_request);
int recvreply(int *network_socket, char *reply);
void recv_command(int socket, command *c);
void recv_entry(int socket, entry *e);
void send_command(int socket, command * c);
void send_entry(int socket, entry *e);
void create_command(command *c, str path);
void send_ACK(int socket, ACK* ack_packet);
void recv_ACK(int socket, ACK *ack_packet);
void send_file(int client_socket, str path);
void recv_file(int socket, str path);

void empty_entry(entry *e);
#endif // NETWORK_H

