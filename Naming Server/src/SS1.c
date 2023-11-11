#include "../inc/cmds.h"


void access_path(entry * e)
{
    // fills the e.paths value with the directories in generate.txt
    FILE* fp = fopen("generate.txt", "r");
    int i = 0;   
    if (fp == NULL) {
        perror("Error opening the file");
        return;
    }
    char line[MAX_PATH_SIZE]; 
    while (fgets(line, sizeof(line), fp) != NULL) {
        strcpy((*e).paths[i++], line);
        (*e).paths[i-1][strlen((*e).paths[i-1]) - 1] = '\0'; // remove newline 
    }
    (*e).entries = i;
    fclose(fp);
}


void server_entry(int id, int cport, str init_path)
{
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(NM_SERVER_PORT);
    int connection_status;
    if(connection(&network_socket, &server_address, &connection_status))
    {
        printf("Error in connection\n"); return;
    }
    entry e;
    e.id = id;
    e.cport = cport;
    strcpy(e.ip, "000.000.010.000");
    e.nmport = NM_SERVER_PORT;  // communication to the NM via the 6060 port
    access_path(&e);  // read all paths from generate.txt, send it to NM
    send_entry(network_socket, &e);
    close(network_socket); 
}

void * handle_client(void * args)
{
    // client gave some command of the form [OPN] [PATH]
    // recieve that command
    int socket = *(int *)args;
    command *c = malloc(sizeof(command));
    recv_command(socket, c);
    printf("SS1 recieved command from :%d, %s \n", socket, c->cmd);
    executeCmd(c, socket);
    printf("sent command %s\n", c->cmd);
    sleep(1);
    free(args);
    close(socket);
}

void * init_server(void * args)
{
    int port = *(int*)args; // SS is listening on this port
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
 
    socklen_t addr_size;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
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
        printf("New request: %d \n", newSocket);
        pthread_create(&t, NULL, handle_client, arg);
    }
}

int main()
{
    // every server will have one thread for handling connection with NM,
    // every server will have one thread for handling connectionS with clients
    // initialize a connection to the NM (NM listens at port 6060 for SS)


    /*-----------------------------------------*/
    int id = 0, port = 6061;
    char path[] = "/home/bassam/Desktop/FP/Storage Server/src";
    /*-----------------------------------------*/
    
    server_entry(id, port, path);
    pthread_t clnt;
    pthread_create(&clnt, 0, init_server, &port);
    pthread_join(clnt, NULL);
    return 0;
}