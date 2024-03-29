#include "../inc/cmds.h"
//#include "../inc/network.h"

#include "../inc/error_codes.h"
#include <dirent.h>
#include "../inc/hash.h"
typedef struct HashMap {
    struct KeyValue* table[MAP_SIZE];
} map_t;
sem_t rwlock;
map_t sent_paths;
int PERMISSIONS, ID, PORT;
entry e;
char* errorMessage[][4] = {
    {COLOR_RED "Error 201: Failed to open the file" RESET, COLOR_RED "Error 202: Failed to write to the file" RESET, NULL},
    {COLOR_RED "Error 301: Failed to open the file" RESET, COLOR_RED "Error 302: Failed to append to the file" RESET, NULL},
    {COLOR_RED "Error 401: Failed to delete the file" RESET, NULL},
    {COLOR_RED "Error 501: Failed to move the file" RESET, NULL},
    {COLOR_RED "Error 601: Failed to get information about the file" RESET,COLOR_RED "Error 602: Informaton buffer smaller than expected" RESET, NULL},
    {COLOR_RED "Error 701: Failed to open the file" RESET, COLOR_RED "Error 702: Failed to allocate memory for reading" RESET, COLOR_RED "Error 703: Failed to read from the file" RESET, NULL},
    {COLOR_RED "Error 801: Failed to create the directory" RESET, NULL},
    {COLOR_RED "Error 901: Failed to open the directory" RESET,COLOR_RED "Error 902: Failed to delete a file within" RESET, COLOR_RED "Error 903: Failed to delete the directory" RESET, NULL},
    {COLOR_RED "Error 111: Failed to open source file" RESET, COLOR_RED "Error 112: Failed to open destination file" RESET, COLOR_RED "Error 113: Failed to copy the content" RESET, NULL},
    {COLOR_RED "Error 121: Failed to open source directory" RESET, COLOR_RED "Error 122: Failed to copy a sub directory" RESET,  COLOR_RED "Error 123: Failed to copy a file within" RESET, NULL},
    {COLOR_RED "Error 101: Permission denied" RESET, NULL},
    {COLOR_RED "Error 102: Invalid command" RESET, NULL},
    {COLOR_RED "Error 103: Invalid arguments for command" RESET, NULL},
    {COLOR_RED "Error 104: Insufficient arguments" RESET, NULL},
    {NULL}
};
int stringcmp(const str s1, const str s2) {
    return !strcmp(s1, s2);
}
void SS_copy_command(int id,int file, str path, command *c)
{
    c->client = -1;
    memset(c->cmd, 0, sizeof(c->cmd));
    strcpy(c->cmd, "copy -x path {SS1/2}");
    c->argc = 4;
    memset(c->argv[0], 0, sizeof(c->argv[0]));
    memset(c->argv[1], 0, sizeof(c->argv[1]));
    memset(c->argv[2], 0, sizeof(c->argv[2]));
    memset(c->argv[3], 0, sizeof(c->argv[3]));

    strcpy(c->argv[0], "copy");
    if(file) strcpy(c->argv[1], "-f");
    else strcpy(c->argv[1], "-d");
    strcpy(c->argv[2], path);
    char temp[5];
    snprintf(temp, sizeof(temp), "SS%d", id);
    strcpy(c->argv[3], temp);
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

void SS_copy(int port, command *c, int backup)
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
        send(network_socket, PARAMS(x));
        str temp_path = calloc(MAX_PATH_SIZE, sizeof(char));
        // target location c->argv[3]
        create_path(temp_path, path);
        command *temp_cmd = malloc(sizeof(command));
        create_command(temp_cmd, temp_path);
        send_command(network_socket, temp_cmd);
        send_file(network_socket, path);
        if(backup) insert(&sent_paths, path, 1);
        free(temp_path);
        free(temp_cmd);
    }
    else if(stringcmp(c->argv[1], "-d"))
    {
        if(backup) send_directory_backup(path, network_socket, &sent_paths);
        else send_directory(path, network_socket);
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
        int x = 1;
        send(socket, PARAMS(x));
        sleep(10);
    }
    close(socket);
}

void send_backup(int port, entry *e)
{
    for(int i=0; i<e->entries; i++)
    {
        if(find(&sent_paths, e->paths[i])) continue;
        command *c = malloc(sizeof(command));
        SS_copy_command(ID, (1-is_directory(e->paths[i])), e->paths[i], c);
        SS_copy(port, c, 1);
        free(c);
    }
    
}

void * NM_handler(void * args)
{
    int socket = *(int *)args;
    
    while(1)
    {
        command *c = malloc(sizeof(command));
        recv_command(socket, c);
        printf("SS%d recieved command from NM in socket :%d, cmd :%s \n",ID, socket, c->cmd);
        if(stringcmp(c->argv[0], "copy") && c->client != PORT)
        {
            SS_copy(c->client, c, 0);
            free(c);
            continue;
        }
        else if(stringcmp(c->argv[0], "backup"))
        {
            send_backup(c->client, &e);
            initHashMap(&sent_paths);
            continue;
        }

       int result = executeCmd(c, socket, PERMISSIONS);
        int fileFlag = 0;
        for(int i =0; i<c->argc; i++)
        {
            if (strcmp(c->argv[i], "-f") == 0)
            {
                fileFlag = 1;
            }
        }
        struct index2D ind = inferCode(c->argv[0], result, fileFlag);
        if(ind.row == -1 && ind.col == -1) printf("Sucessful execution\n");
        else
            printf("%s\n", errorMessage[ind.row][ind.col]);

        printf("sent command %s\n", c->cmd);
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
    e.id = id;
    e.cport = cport;
    strcpy(e.ip, "000.000.010.000");
    e.nmport = NM_SERVER_PORT;  // communication to the NM via the 6060 port
    access_path(&e);  // read all paths from generate.txt, send it to NM
    e.permissions = PERMISSIONS;
    send_entry(network_socket, &e);
    if(ID  <= 2) return network_socket;
    int create_bkup = 0;
    recv(network_socket, PARAMS(create_bkup)); // containts port
    if(create_bkup)
    {
        send_backup(create_bkup, &e);
        initHashMap(&sent_paths);
    }
    recv(network_socket, PARAMS(create_bkup)); // containts port
    printf("got %d\n", create_bkup);
    if(create_bkup)
    {
        send_backup(create_bkup, &e);
        initHashMap(&sent_paths);
    }
    return network_socket;
}

void print_cmd(command *c)
{
    printf("argc : %d, argv[0] : %s, argv[1] : %s, argv[2] : %s, argv[3] : %s\n", c->argc, c->argv[0],c->argv[1],c->argv[2],c->argv[3]);
}
void handle_SS(int socket, command *cmd) // destination SS
{
    printf("Reached inside handle_SS for SS%d for cmd %s\n", ID, cmd->argv[cmd->argc - 1]);
    int operation = -1;
    sem_wait(&rwlock);
    while(1)
    {
        recv(socket, PARAMS(operation));
        if(operation == MKFIL)
        {
            printf("Recieved makefile\n");
            command * c = malloc(sizeof(command));
            recv_command(socket, c);
            char full_path[MAX_PATH_SIZE];
            memset(full_path, 0, sizeof(full_path));
            snprintf(full_path, sizeof(full_path), "%s/%s", cmd->argv[cmd->argc - 1], c->argv[c->argc-1]);
            printf("recieved file %s\n", c->argv[c->argc - 1]);
            printf("full_path : %s\n", full_path);
            recv_file(socket, full_path);
            free(c);
        }
        else if(operation == MKDIR)
        {
            printf("Recieved makedir\n");
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
    sem_post(&rwlock);
    printf("broke at %d\n", operation);
}

struct index2D inferCode(const char* cmd, int code, int fileFlag)
{
    struct index2D ind;
    if(code == 0)
    {
        ind.col = -1;
        ind.row = -1;
        return ind;
    }
    int length = 15;
    // while (errorMessage[length][0] != NULL) {
    //     length++;
    // }

    if (code < 0) 
    {
        ind.row = length+code-1;
        ind.col = 0;
        return ind;
    }

    ind.col = code-1;
    if (strcmp(cmd, "write") == 0 && fileFlag)
    {
        ind.row = 0;
    }
    else if (strcmp(cmd, "append"))
    {
        ind.row = 1;
    }
    else if (strcmp(cmd, "delete") == 0 && fileFlag)
    {
        ind.row = 2;
    }
    else if(strcmp(cmd, "move") == 0 && fileFlag)
    {
        ind.row = 3;
    }
    else if(strcmp(cmd, "getinfo") == 0)
    {
        ind.row = 4;
    }
    else if(strcmp(cmd, "read") == 0)
    {
        ind.row = 5;
    }
    else if(strcmp(cmd, "create") == 0 && !fileFlag)
    {
        ind.row = 6;
    }
    else if(strcmp(cmd, "delete") == 0 && !fileFlag)
    {
        ind.row = 7;
    }
    else if(strcmp(cmd, "copy") == 0 && fileFlag)
    {
        ind.row = 8;
    }
    else if(strcmp(cmd, "copy") == 0 && !fileFlag)
    {
        ind.row = 9;
    }
    return ind;
}



void * handle_client(void * args)
{
    // client gave some command of the form [OPN] [PATH]
    // recieve that command
    int socket = *(int *)args;
    command *c = malloc(sizeof(command));
    recv_command(socket, c);
    if(c->client == SUDOC)
    {
        handle_SS(socket, c);
        free(args);
        close(socket);
        return;
    }
    printf("SS%d recieved command from socket :%d, client : %d, %s \n",ID, socket,c->client, c->cmd);
    int result = executeCmd(c, socket, PERMISSIONS);
    int fileFlag = 0;
    for(int i =0; i<c->argc; i++)
    {
        if (strcmp(c->argv[i], "-f") == 0)
        {
            fileFlag = 1;
        }
    }
    struct index2D ind = inferCode(c->argv[0], result, fileFlag);
    if(ind.row == -1 && ind.col == -1) printf("Sucessful execution\n");
    else
        printf("%s\n", errorMessage[ind.row][ind.col]);
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
const int MAX_IP_SIZE = 512;

int main(int argc, char *argv[]) {
	char IP[MAX_IP_SIZE];

    if (argc != 5) {
        printf("Usage: %s <ID> <PORT> <PERMISSIONS> <IP>\n", argv[0]);
        return 1;
    }

    ID = atoi(argv[1]);
    PORT = atoi(argv[2]);
    PERMISSIONS = atoi(argv[3]);
    strncpy(IP, argv[4], MAX_IP_SIZE - 1);
    IP[MAX_IP_SIZE - 1] = '\0';
    
    char path[] = "/home/bassam/Desktop/FP/Storage Server/src";

    // The rest of your code remains unchanged, using ID, PORT, PERMISSIONS, and IP as needed.
    initHashMap(&sent_paths);   
    int nm_sock = server_entry(ID, PORT, path); //IP?
    pthread_t clnt, serv, nm;
    rw_lock_t rwlockk;
    rw_lock_init(&rwlockk, &rwlock);
    pthread_create(&serv, NULL, NM_alive, &nm_sock);
    pthread_create(&nm, NULL, NM_handler, &nm_sock);
    pthread_create(&clnt, NULL, init_server, &PORT);
    pthread_join(clnt, NULL);

    return 0;
}

// int main()
// {
//     // every server will have one thread for handling connection with NM,
//     // every server will have one thread for handling connectionS with clients
//     // initialize a connection to the NM (NM listens at port 6060 for SS)


//     /*-----------------------------------------*/
//     int id = 1, port = 6061, permissions = (1<<9) - 1;
//     PERMISSIONS = permissions;
//     PORT = port;

//     ID = id;
//     // PERMISSIONS ^= RED; // disable read permission
//     char path[] = "/home/bassam/Desktop/FP/Storage Server/src";
//     /*-----------------------------------------*/
    
//     int nm_sock = server_entry(id, port, path);
//     pthread_t clnt, serv, nm;
//     pthread_create(&serv, 0, NM_alive, &nm_sock);
//     pthread_create(&nm, 0, NM_handler, &nm_sock);
//     pthread_create(&clnt, 0, init_server, &port);
//     pthread_join(clnt, NULL);
//     return 0;
// }
