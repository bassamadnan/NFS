#include "../inc/cmds.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "../inc/hash.h"
#include "../inc/lru.h"

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
void trim_path(str path)
{
    int len = strlen(path) - 1; 
    int i=0;
    for(; i<len + 1; i++)
    {
        if(path[i] == '/') break;
    }
    if (i == len + 1) return;
    while(len && (path[len] != '/'))
    {
        path[len] = '\0';
        len--;
    }
    if(len) path[len] = '\0';
}

void create_path(str temp_path, str path)
{
    int len = strlen(path); 
    int i= len - 1;
    for(; i>=0; i--)
    {
        if(path[i] == '/') break;
    }
    if(i == 0)
    {
        strcat(temp_path, path);
        return;
    }
    while(i < len)
    {
        char curr[2];
        curr[1] = '\0';
        curr[0] = path[i];
        strcat(temp_path, curr);
        i++;
    }
}

LRUCache* cache;


serverstat SS_stat[MAX_ENTRIES]; // 0 if not connected, 1 if connected

typedef struct HashMap {
    struct KeyValue* table[MAP_SIZE];
} map_t;

map_t ID; // maps the entries of an SS to their IDs, insertion find ->logn
pthread_t tid[60];
entry entries[MAX_ENTRIES];

void log_network_operation(const char *details) {
    FILE *fp = fopen("log.txt", "a"); // Open log file in append mode
    if (fp != NULL) {
        fprintf(fp, "%s\n", details); // Write details to the log file
        fclose(fp);
    } else {
        printf("Error opening log file!\n");
    }
}

int find_SS(str full_path, int trim)
{
    // Finds the SS the command is intended from
    // c->argv[argc - 1] contains the path, we need to check the last '/'
    str path = calloc(MAX_PATH_SIZE, sizeof(char));
    strcpy(path, full_path);
    if(trim) trim_path(path);
    // use find here
    if(!path)
    {
        free(path);
        return 0;
    }
    printf("before\n");
    struct CacheEntry catch = retrieveEntry(cache, path);
    printf("after %s\n", path);
    int id = 0;
    if(catch.serverID == -1){
        id = find(&ID, path); // logn
        printf("cache miss: %d\n", id);
        if(id)
        {
            addFile(cache, path, id);
        } 
    }
    else
    {
        id = catch.serverID;
        printf("cache hit: %d\n", id);
    }
    if(id)
        printf("valid for %d,  path: %s\n", id, path);
    else 
        printf("invalid for %d,  path: %s\n", id, path);

    free(path);
    return id;

}

int check_SS(id)
{
    return SS_stat[id].isalive;
}

void NM_connect(int id, command *c)
{
    int socket = SS_stat[id].socket;
    send_command(socket, c);
    char log_details[200];
    sprintf(log_details, "SS id: %d, port: %d, ip: %s", id, SS_stat[id].port, entries[id].ip);
    log_network_operation(log_details);
}

void * client_function(int * x)
{
    int client_socket = *x;
    free(x);
    
    for(;;)
    {
        command *c = malloc(sizeof(command));
        recv_command(client_socket, c);
        printf("Recieved \n");
        int flag;
        if((flag = privileged_cmd(c)) != 0)
        {
            if(flag == 3)
            {
                // copy path1 to path2 without trim
                int id1 = find_SS(c->argv[c->argc - 1], 0); // dest
                int id2 = find_SS(c->argv[c->argc - 2], 0); // src
                if(!(check_SS(id1) && check_SS(id2))) continue; // error code here
                c->client = entries[id1].cport;
                if((entries[id2].permissions & CPY) == 0)
                {
                    printf("Copy not allowed for requested SS%d\n", id2);
                    continue;
                }
                NM_connect(id2, c);
                // SSid1 sends data to SSid2 (acts as client?)
                char log_details[200];
                sprintf(log_details, "Sent command from SS%d to SS%d (Copy operation)", id2, id1);
                log_network_operation(log_details);
                
            }
            if(flag == 2)
            {
                int id = find_SS(c->argv[c->argc - 1], 1);
                if(!check_SS(id)) continue; // error code here
                // create a file in this SS
                NM_connect(id, c);
            }
            if(flag == 1)
            {
                int id = find_SS(c->argv[c->argc - 1], 0);
                if(!check_SS(id)) continue; // error code here
                // create a file in this SS
                NM_connect(id, c);
            }
            continue;
        }
        // not a privillegd cmd, read/write etc, find an SS and send it back to the client
        int argc = c->argc;
        char path[MAX_PATH_SIZE];
        memset(path, 0, sizeof(path));
        strcpy(path, c->argv[argc - 1]);

        int ss = find_SS(path, 0);

        entry e;
        empty_entry(&e);
        if(ss)
        {
            printf("Found in ID: %d, listening on port %d, ip: %s\n", entries[ss].id, entries[ss].cport, entries[ss].ip);
            e = entries[ss];
        }
        else{
            printf("Client %d requested for %s\n", client_socket, path);
            printf("Not found\n");
            return;
        }
        char log_details[200];
        sprintf(log_details, "Found SS for %s on id: %d, listening on port: %d, ip: %s", path, ss, entries[ss].cport, entries[ss].ip);
        log_network_operation(log_details);

        send_entry(client_socket, &e);
    }
    close(client_socket);
}

void server_function(int * x)
{
    int SS_socket = *x;
    free(x);
    entry *e = malloc(sizeof(entry));
    empty_entry(e);
    recv_entry(SS_socket, e);
    check_reconnect(e->id, SS_stat, entries);
    int i = 0;
    printf("id: %d, entries: %d,cport: %d, nmport: %d, ip %s perms: %d\n", e->id, e->entries, e->cport, e->nmport, e->ip, e->permissions);
    entries[e->id] = *e;

    SS_stat[e->id] = (serverstat){
        .socket = SS_socket,
        .isalive = 1,
        .port = e->cport
    };
    if(e->id > 2)
    {
        init_backup(e->id, SS_stat);
    }
    while(i<e->entries)
    {
        insert(&ID, e->paths[i], e->id);
        printf("%s\n", e->paths[i++]);
    }
    
    while(1)
    {
        // sleep(1);
        // int x = 0;
        // recv(SS_socket, PARAMS(x));
        // printf("Recieved : %d\n", x);
        // if(!x)
        // {
        //     SS_stat[e->id].isalive = 0;
        //     break;
        // }
        //are we not doing this or what
        
        //wherever we do this put this logging
        /*
        char log_details[200];
        sprintf(log_details, "Received data from SS with id: %d", e->id);
        log_network_operation(log_details);
        */
    }
    close(SS_socket);
}

void * client_thread(void * args)
{
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
 
    socklen_t addr_size;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(NM_CLIENT_PORT);
 
    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
 
    if (listen(serverSocket, 50) == 0)
        printf("Listening\n");
    else
        printf("Error\n");
    
    while(1)
    {
        addr_size = sizeof(serverAddr);
        newSocket = accept(serverSocket, (struct sockaddr*)&serverStorage, &addr_size);
        pthread_t t;
        int *arg = malloc(sizeof(int));
        *arg = newSocket;
        printf("New Client: %d\n", newSocket);
        pthread_create(&t, NULL, client_function, arg);
        char log_details[200];
        sprintf(log_details, "Accepted new client: %d", newSocket);
        log_network_operation(log_details);
        
    }
}

void * server_thread(void * args)
{
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
 
    socklen_t addr_size;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(NM_SERVER_PORT);
    bind(serverSocket,(struct sockaddr*)&serverAddr, sizeof(serverAddr));

    if (listen(serverSocket, 50) == 0)
        printf("Listening\n");
    else
        printf("Error\n");
 
    
    while(1)
    {
        addr_size = sizeof(serverAddr);
        newSocket = accept(serverSocket,(struct sockaddr*)&serverStorage,&addr_size);
        pthread_t t;
        int *arg = malloc(sizeof(int));
        *arg = newSocket;
        printf("New server: %d \n", newSocket);
        pthread_create(&t, NULL, &server_function, arg);
        char log_details[200];
        sprintf(log_details, "Accepted new server: %d", newSocket);
        log_network_operation(log_details);
    }
}


int main()
{
	log_network_operation("Initialized networking threads");
    // sem_init(&clientLock, 0, 10);
    cache = createCache(CACHE_SIZE);
    pthread_t cth, sth;
    initHashMap(&ID);
    pthread_create(&cth, NULL, client_thread, NULL);
    pthread_create(&sth, NULL, server_thread, NULL);
    pthread_join(cth, NULL);
    pthread_join(sth, NULL);
    return 0;
}
