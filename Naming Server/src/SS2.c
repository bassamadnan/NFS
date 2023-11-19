#include "../inc/cmds.h"
//#include "../inc/network.h"
int PERMISSIONS, ID;

void access_path(entry * e)
{
    // fills the e.paths value with the directories in generate.txt
    FILE* fp = fopen("c.txt", "r");
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

void SS_copy(int port, command *c)
{
    // connect to SS listening on port 
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);
    int connection_status;
    if(connection(&network_socket, &server_address, &connection_status))
    {
        printf("Error in connection to server listening on port %d\n", port); return;
    }
    // 
    c->client = SUDOC;
    send_command(network_socket, c);
    str path = calloc(MAX_PATH_SIZE, sizeof(char));
    strcpy(path, c->argv[2]);
    if(stringcmp(c->argv[1], "-f"))
    {
        int x = MKFIL;
        send(x, PARAMS(x));
        send_command(network_socket, c);
        send_file(network_socket, path);
    }
    else
    {
        send_directory(path, network_socket);
    }
    free(path);
    int END_CONNECTION = 2504;
    send(network_socket, PARAMS(END_CONNECTION));
}

void * NM_alive(void * args) // sends a packet every 1 second to show its still connected
{
    int socket = *(int *)args;
    while(1)
    {
        // command *c = malloc(sizeof(command));
        // recv_command(socket, c);
        // printf("SS1 recieved command from NM on socket : %d, %s \n", socket, c->cmd);
        // executeCmd(c, socket);
        // free(c);
        // int x = 1;
        // send(socket, PARAMS(x));
        // sleep(1);
    }
    close(socket);
}

void * NM_handler(void * args)
{
    int socket = *(int *)args;
    
    while(1)
    {
        command *c = malloc(sizeof(command));
        recv_command(socket, c);
        printf("SS%d recieved command from NM in socket :%d, cmd :%s \n",ID, socket, c->cmd);
        if(stringcmp(c->argv[0], "copy"))
        {
            SS_copy(c->client, c);
            free(c);
            continue;
        }
        executeCmd(c, socket, PERMISSIONS);
        printf("recieved command %s\n", c->cmd);
        free(c);
    }
}

int server_entry(int id, int cport, str init_path)
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
        printf("Error in connection\n"); return -1;
    }
    entry e;
    e.id = id;
    e.cport = cport;
    strcpy(e.ip, "000.000.010.000");
    e.nmport = NM_SERVER_PORT;  // communication to the NM via the 6060 port
    access_path(&e);  // read all paths from generate.txt, send it to NM
    send_entry(network_socket, &e);
    return network_socket;
}

void handle_SS(int socket, command *cmd) // destination SS
{
    printf("Reached inside handle_SS for SS%d for cmd %s\n", ID, cmd->argv[cmd->argc - 1]);
    while(1)
    {
        int operation = -1;
        recv(socket, PARAMS(operation));
        if(operation == MKFIL)
        {
            command * c = malloc(sizeof(command));
            recv_command(socket, c);
            char full_path[MAX_PATH_SIZE];
            memset(full_path, 0, sizeof(full_path));
            snprintf(full_path, sizeof(full_path), "%s/%s", cmd->argv[cmd->argc - 1], c->argv[c->argc-1]);
            printf("recieved file %s\n", c->argv[c->argc - 1]);
            recv_file(socket, full_path);
            free(c);
        }
        else if(operation == MKDIR)
        {
            command * server_cmd = malloc(sizeof(command));
            recv_command(socket, server_cmd);
            char full_path[MAX_PATH_SIZE];
            snprintf(full_path, sizeof(full_path), "%s/%s", cmd->argv[cmd->argc - 1], server_cmd->argv[2]);
            memset(server_cmd->argv[2], 0, sizeof(server_cmd->argv[2]));
            strcpy(server_cmd->argv[2], full_path);
            printf("Recieved directory %s\n", server_cmd->argv[2]);
            executeCmd(server_cmd, socket, PERMISSIONS);
            free(server_cmd);
        }
        else break;
    }
}

void * handle_client(void * args)
{
    // client gave some command of the form [OPN] [PATH]
    // recieve that command
    int socket = *(int *)args;
    command *c = malloc(sizeof(command));
    recv_command(socket, c);
    if(c->client == -1)
    {
        handle_SS(socket, c);
        free(args);
        close(socket);
        return;
    }
    printf("SS%d recieved command from socket :%d, client : %d, %s \n",ID, socket,c->client, c->cmd);
    executeCmd(c, socket, PERMISSIONS);
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
    int id = 2, port = 6062, permissions = ~(1<<8);
    PERMISSIONS = permissions;
    ID = id;
    // PERMISSIONS ^= RED; // disable read permission
    char path[] = "/home/bassam/Desktop/FP/Storage Server/src";
    /*-----------------------------------------*/
    int nm_sock = server_entry(id, port, path);
    pthread_t clnt, serv, nm;
    pthread_create(&serv, 0, NM_alive, &nm_sock);
    pthread_create(&nm, 0, NM_handler, &nm_sock);
    pthread_create(&clnt, 0, init_server, &port);
    pthread_join(clnt, NULL);
    return 0;
}
