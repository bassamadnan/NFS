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
    int port;
}serverstat;

// void updateEntry();
int check_reconnect(int id, serverstat *SS_stat, entry *e);
void init_backup( int id, serverstat *SS_stat);
void init_SS12(serverstat *SS_stat);
//#include "hash.h"
