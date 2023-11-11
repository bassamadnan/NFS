#include "../inc/cmds.h"

entry temp;

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
        printf("Error in connection\n"); return;
    }
    send_command(network_socket, c);
    char response[MAX_INPUT_SIZE];
    memset(response, 0, sizeof(response));
    sleep(1);
    int x = recv(network_socket, response, sizeof(response), 0);
    printf("recieved %d response: \n%s\n", x, response);
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
        entry *e = malloc(sizeof(entry)); // free this later !!
        empty_entry(e);
        recv_entry(network_socket, e);
        printf("In port : %d\n", e->cport);
        SS_connect(e->cport, &c);

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