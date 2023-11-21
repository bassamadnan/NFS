#include "../inc/cmds.h"
#include "../inc/hash.h"

typedef struct HashMap {
    struct KeyValue* table[MAP_SIZE];
} map_t;

// is this id reconnecting?
int check_reconnect(int id, serverstat *SS_stat, entry *e)
{
    if(SS_stat[id].socket != 0)
    {
        SS_stat[id].isalive = 1;
        printf("SS%d reconnected\n", id);
    }
    else
    {
        printf("New connection : SS%d\n", id);
        SS_stat[id].isalive = 1;
        SS_stat[id].socket = 0;
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
void backup_command(int id, command *c)
{
    c->client = 1;
    memset(c->cmd, 0, sizeof(c->cmd));
    strcpy(c->cmd, "backup -d {SS_id}");
    c->argc = 3;
    memset(c->argv[0], 0, sizeof(c->argv[0]));
    memset(c->argv[1], 0, sizeof(c->argv[1]));
    memset(c->argv[2], 0, sizeof(c->argv[3]));
    strcpy(c->argv[0], "backup");
    strcpy(c->argv[1], "-d");
    char temp[5];
    snprintf(temp, sizeof(temp), "SS%d", id);
    strcpy(c->argv[2], temp);
}

void backup_SS1(int id, serverstat *SS_stat)
{
    // backup SS1 into SSid
    if(SS_stat[id].isalive == 0) return;
    command *c = malloc(sizeof(command));
    makedir_command(1, c);
    send_command(SS_stat[id].socket, c);
    free(c);
    command *c1 = malloc(sizeof(command));
    backup_command(1, c1);
    c1->client = SS_stat[id].port;
    send_command(SS_stat[id].socket, c1);
    free(c1);
}

void backup_SS2(int id, serverstat *SS_stat)
{
    if(SS_stat[id].isalive == 0) return;
    command *c = malloc(sizeof(command));
    makedir_command(2, c);
    send_command(SS_stat[id].socket, c);
    free(c);
    command *c1 = malloc(sizeof(command));
    backup_command(2, c1);
    c1->client = SS_stat[id].port;
    send_command(SS_stat[id].socket, c1);
    free(c1);
}
void init_SS12(serverstat *SS_stat)
{
    backup_SS1(2, SS_stat);
    sleep(4);
    backup_SS1(3, SS_stat);
    sleep(3);
    backup_SS2(1, SS_stat);
    sleep(4);
    backup_SS2(3, SS_stat);
    sleep(3);
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
    int SS1_port = SS_stat[1].port;
    int SS2_port = SS_stat[2].port;
    SS_stat[id].backup = 1;
    send(SS_stat[id].socket, PARAMS(SS1_port));
    send(SS_stat[id].socket, PARAMS(SS2_port));
    // create_backup(id, SS_stat);
    free(c);
}

// facilitate teh copy of each of the entries of id

void create_copy(int SS1_socket, int SS2_socket, int id, entry *e)
{
    // e[]

}

void generate_command(int id, command *c1, command *c2)
{
    c2->argc = c1->argc;
    c2->client = c1->client;
    memset(c2->cmd, 0, sizeof(c2->cmd));
    strcpy(c2->cmd, c1->cmd);
    for(int i=0; i<c1->argc; i++)
    {
        memset(c2->argv[i], 0, sizeof(c2->argv[i]));
        strcpy(c2->argv[i], c1->argv[i]);
    }
    memset(c2->argv[c2->argc - 1], 0, sizeof(c2->argv[c2->argc - 1]));
    snprintf(c2->argv[c2->argc - 1], sizeof(c2->argv[c2->argc -1]), "SS%d/%s", id, c1->argv[c1->argc - 1]);
}

void exec_backup(int id, serverstat *SS_stat , command *c)
{
    // execute the same command on backup
    if(SS_stat[1].isalive)
    {
        // write <conents> path -> write <conents> SS1/path
        // path in argv[argc-1] 
        int sket = SS_stat[1].socket;
        command *newcmd = malloc(sizeof(command));
        generate_command(id, c, newcmd);
        send_command(sket, newcmd);
        free(newcmd);
    }
    if(SS_stat[2].isalive)
    {
        // write <conents> path -> write <conents> SS1/path
        // path in argv[argc-1] 
        int sket = SS_stat[2].socket;
        command *newcmd = malloc(sizeof(command));
        generate_command(id, c, newcmd);
        send_command(sket, newcmd);
        free(newcmd);
    }
}