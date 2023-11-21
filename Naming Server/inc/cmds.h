#include <unistd.h>
#include <pthread.h>
#include "commands.h"
#include "defs.h"
#include "network.h"
#include "file_operations.h"
#include "command_execution.h"
#include "lock.h"

typedef struct severstat
{
    int socket; // socket for the NM to communicate with this 
    int isalive;
    int backup;
    int port;
}serverstat;

// void updateEntry();
void exec_backup(int id, serverstat *SS_stat , command *c);
int check_reconnect(int id, serverstat *SS_stat, entry *e);
void init_backup( int id, serverstat *SS_stat);
void init_SS12(serverstat *SS_stat);
//#include "hash.h"

#define COLOR_RED     "\x1b[31m"
#define RESET   "\x1b[0m"

struct index2D inferCode(const char* cmd, int code, int fileFlag);
