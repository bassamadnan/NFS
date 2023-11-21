#include "../inc/cmds.h"

void check_reconnect(int id, serverstat *SS_stat, entry *e)
{
    if(SS_stat[id].socket != 0)
    {
        SS_stat[id].isalive = 1;
        printf("SS%d reconnected\n", id);
    }
    else
    {
        printf("New connection : SS%d\n", id);
    }
}
