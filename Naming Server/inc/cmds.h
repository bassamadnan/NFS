#include <unistd.h>
#include <pthread.h>
#include "commands.h"
#include "defs.h"
#include "network.h"
#include "file_operations.h"
#include "command_execution.h"
int stringcmp(const str s1, const str s2) {
    return !strcmp(s1, s2);
}

int privileged_cmd(command *c)
{
    if(stringcmp(c->argv[0], "move") || stringcmp(c->argv[0], "copy")) return 3;
    if(stringcmp(c->argv[0], "create")) return 2;
    if(stringcmp(c->argv[0], "delete")) return 1; 
    return 0;
}
//#include "hash.h"
