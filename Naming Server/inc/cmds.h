#include <unistd.h>
#include <pthread.h>
#include "commands.h"
#include "defs.h"
#include "network.h"
#include "file_operations.h"
#include "command_execution.h"
//#include "redundancy.h"




typedef struct severstat
{
    int socket; // socket for the NM to communicate with this 
    int isalive;
    int backup;
}serverstat;

// void updateEntry();
void check_reconnect(int id, serverstat *SS_stat, entry *e);
void create_backup(int SS1_socket, int SS2_socket, int id, serverstat *SS_stat);
//#include "hash.h"
