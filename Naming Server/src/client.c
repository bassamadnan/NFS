#include "../inc/cmds.h"

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
    if(!(stringcmp(c->argv[0], "read") || stringcmp(c->argv[0], "getinfo")))
    {
        // expect ack
        ACK *ack = malloc(sizeof(ACK));
        recv_ACK(network_socket, ack);
        printf("STATUS : id : %d, code :%d\n", ack->id, ack->code);
        // free(network_socket);

        return;
    }
    if(stringcmp(c->argv[0], "read"))
    {
        read_file(network_socket);
    }
    // free(network_socket);
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
