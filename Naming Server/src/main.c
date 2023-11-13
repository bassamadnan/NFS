#include "../inc/cmds.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

pthread_t tid[60];
entry entries[MAX_ENTRIES];


void * client_function(int * x)
{
    int client_socket = *x;
    free(x);
    
    for(;;)
    {
        command c;
        recv_command(client_socket, &c);
        printf("Recieved \n");
        int argc = c.argc;
        char path[MAX_PATH_SIZE];
        memset(path, 0, sizeof(path));
        strcpy(path, c.argv[argc - 1]);
        int ss = -1;
        for(int i=0; i < MAX_ENTRIES; i++)
        {
            int id = 0;
            while(id < entries[i].entries)
            {
                // printf("Compared %s with %s\n",path, entries[i].paths[id]);
                if(strcmp(path, entries[i].paths[id]) == 0)
                {
                    ss = i;
                    break;
                }
                id++;
            }
        }
        entry e;
        empty_entry(&e);
        if(ss != -1)
        {
            printf("Found in ID: %d, listening on port %d, ip: %s\n", entries[ss].id, entries[ss].cport, entries[ss].ip);
            e = entries[ss];
        }
        else{
            printf("Client %d requested for %s\n", client_socket, path);
            printf("Not found\n");
        }
        send_entry(client_socket, &e);
    }
    close(client_socket);
}
void server_function(int * x)
{
    int client_socket = *x;
    free(x);
    entry *e = malloc(sizeof(entry));
    empty_entry(e);
    recv_entry(client_socket, e);
    int i = 0;
    printf("id: %d, entries: %d,cport: %d, nmport: %d, ip %s from thread: 1\n", e->id, e->entries, e->cport, e->nmport, e->ip);
    while(i<e->entries)
    {
        printf("%s\n", e->paths[i++]);
    }
    entries[e->id] = *e;
    close(client_socket);
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
    }

}
int main()
{
    // sem_init(&clientLock, 0, 10);
    pthread_t cth, sth;
    pthread_create(&cth, NULL, client_thread, NULL);
    pthread_create(&sth, NULL, server_thread, NULL);
    pthread_join(cth, NULL);
    pthread_join(sth, NULL);
    return 0;
}
