#include "../inc/cmds.h"

pthread_t tid[60];

void * client_function(int * x)
{
    int client_socket = *x;
    free(x);
    
    while(1)
    {
        char buffer[1024];
        size_t bytes_read;
        int msg_size = 0;
        char actualpath[4086];
        while((bytes_read = read(client_socket, buffer + msg_size, sizeof(buffer) - msg_size - 1)) > 0)
        {
            msg_size += bytes_read;
            if(msg_size > 1024 -1 || buffer[msg_size - 1] == '\0') break;
        }
        buffer[msg_size - 1] = '\0';
        printf("REQUEST: %s from %d\n", buffer, client_socket);
        fflush(stdout);
        FILE *fp = fopen(buffer, "r");
        // read contents of file and send them to client
        while((bytes_read = fread(buffer, 1 , 1024, fp)) > 0)
        {
            write(client_socket, buffer, bytes_read);
        }
        fclose(fp);
        memset(buffer, '\0', sizeof(buffer));
    }
    
    close(client_socket);
}

void * server_function(int * x)
{
    int client_socket = *x;
    free(x);
    entry e;
    read(client_socket, &e, sizeof(e));
    int i = 0;
    while(i<e.entries)
    {
        printf("%s\n", e.paths[i++]);
    }
    
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
    serverAddr.sin_port = htons(5050);
 
    bind(serverSocket,
         (struct sockaddr*)&serverAddr,
         sizeof(serverAddr));
 

    if (listen(serverSocket, 50) == 0)
        printf("Listening\n");
    else
        printf("Error\n");
 
    
    while(true)
    {

        addr_size = sizeof(serverAddr);

        newSocket = accept(serverSocket,
                           (struct sockaddr*)&serverStorage,
                           &addr_size);

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
    serverAddr.sin_port = htons(6060);
 
    bind(serverSocket,
         (struct sockaddr*)&serverAddr,
         sizeof(serverAddr));
 

    if (listen(serverSocket, 50) == 0)
        printf("Listening\n");
    else
        printf("Error\n");
 
    
    while(true)
    {

        addr_size = sizeof(serverAddr);

        newSocket = accept(serverSocket,
                           (struct sockaddr*)&serverStorage,
                           &addr_size);

        pthread_t t;
        int *arg = malloc(sizeof(int));
        *arg = newSocket;
        printf("New server: %d \n", newSocket);
        pthread_create(&t, NULL, server_function, arg);

    }
}
int main()
{
    pthread_t cth, sth;
    pthread_create(&cth, NULL, client_thread, NULL);
    pthread_create(&sth, NULL, server_thread, NULL);
    pthread_join(cth, NULL);
    pthread_join(sth, NULL);
    return 0;
}