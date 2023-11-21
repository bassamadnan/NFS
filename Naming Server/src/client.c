#include "../inc/cmds.h"

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

entry temp;

void read_file(int socket) {
    
    char buffer[BUFFER_SIZE];
    int bytes_received, x= 0;
    int y;
    recv(socket, PARAMS(y));
    send(socket, PARAMS(y));
    printf("expecting %d bytes\n", y);

    while (x < y && (bytes_received = recv(socket, buffer, 1, 0)) > 0) {
        // printf("recved:%d , buffer: %s x\n",bytes_received, buffer);
        printf("%s", buffer);
        x += bytes_received;
    }
    printf("\n");
}

void SS_connect(int port, command *c)
{
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
    send_command(network_socket, c);
    // if(!(stringcmp(c->argv[0], "read") || stringcmp(c->argv[0], "getinfo")))
    // {
    //     // expect ack
    //     ACK *ack = malloc(sizeof(ACK));
    //     recv_ACK(network_socket, ack);
    //     // printf("STATUS : id : %d, code :%d\n", ack->id, ack->code);
    //     printf("%s\n", errorMessage[ack->id][ack->code]);
    //     // free(network_socket);

    //     return;
    // }
    // if(stringcmp(c->argv[0], "read"))
    // {
    //     read_file(network_socket);
    // }
    // free(network_socket);
        ACK *ack = malloc(sizeof(ACK));
        recv_ACK(network_socket, ack);
        // printf("STATUS : id : %d, code :%d\n", ack->id, ack->code);
        printf("%s\n", errorMessage[ack->id][ack->code]);
}


void *clienthread(void * args)
{
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(NM_CLIENT_PORT);
    int connection_status;
    if(connection(&network_socket, &server_address, &connection_status))
    {
        printf("Error in connection\n"); return 0;
    }
    while(1)
    {
        command c = parser(network_socket);
        send_command(network_socket, &c);
        if(privileged_cmd(&c)) continue; // wait for ACK just conitnue for now
        entry *e = malloc(sizeof(entry)); // free this later !!
        empty_entry(e);
        recv_entry(network_socket, e);

        if(e->id != -1) SS_connect(e->cport, &c);
        if(e->id == -1) printf("from NM: bad request \n");

    }
    close(network_socket);
 
}

int main()
{
    pthread_t c;
    pthread_create(&c, NULL, clienthread, NULL);
    pthread_join(c, NULL);
    return 0;
}
