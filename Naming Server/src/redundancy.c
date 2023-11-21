#include "../inc/cmds.h"
#include "../inc/hash.h"

typedef struct HashMap {
    struct KeyValue* table[MAP_SIZE];
} map_t;
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

void makedir_command(int id, command *c)
{
    c->client = 1;
    memset(c->cmd, 0, sizeof(c->cmd));
    strcpy(c->cmd, "create -d {SS_id}");
    c->argc = 3;
    memset(c->argv[0], 0, sizeof(c->argv[0]));
    memset(c->argv[1], 0, sizeof(c->argv[1]));
    memset(c->argv[2], 0, sizeof(c->argv[3]));
    strcpy(c->argv[0], "create");
    strcpy(c->argv[1], "-d");
    char temp[5];
    snprintf(temp, sizeof(temp), "SS%d", id);
    strcpy(c->argv[2], temp);
}
void copy_command(int id, command *c)
{
    c->client = 1;
    memset(c->cmd, 0, sizeof(c->cmd));
    strcpy(c->cmd, "copy -d {SS_id}");
    c->argc = 3;
    memset(c->argv[0], 0, sizeof(c->argv[0]));
    memset(c->argv[1], 0, sizeof(c->argv[1]));
    memset(c->argv[2], 0, sizeof(c->argv[3]));
    strcpy(c->argv[0], "create");
    strcpy(c->argv[1], "-d");
    char temp[5];
    snprintf(temp, sizeof(temp), "SS%d", id);
    strcpy(c->argv[2], temp);
}

void backup_SS1(int id, serverstat *SS_stat)
{
    int socket = SS_stat[id].socket; 
    // create
}

void create_backup(int id, serverstat *SS_stat)
{
    // first create backup in SS1
    if(SS_stat[1].isalive) backup_SS1(id, SS_stat);    

}

void init_backup(int id, serverstat *SS_stat)
{
    // first time SS connects, make a directory in SS1 and SS2 
    if(SS_stat[id].backup)
    {
        int dont = 0;
        send(SS_stat[id].socket, PARAMS(dont)); // dont send backup
        return;
    }
    command *c = malloc(sizeof(command));
    makedir_command(id, c);
    send_command(SS_stat[1].socket, c); // ss1
    send_command(SS_stat[2].socket, c); // ss2
    SS_stat[id].backup = 1;
    int send_bkup = 1;
    send(SS_stat[id].backup, PARAMS(send_bkup));
    create_backup(id, SS_stat);
    free(c);
}

// facilitate teh copy of each of the entries of id

void create_copy(int SS1_socket, int SS2_socket, int id, entry *e)
{
    // e[]

}