#include "../inc/cmds.h"


void* clienthread()
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
        char client_request[1000];
        printf("Enter txt filename: ");
        scanf("%s", client_request);
        client_request[strlen(client_request)] = '\0';
        send(network_socket, client_request, PARAMS(client_request));
        char reply[4096];
        recv(network_socket, reply, PARAMS(reply));
        printf("%s\n", reply);
        fflush(stdout);

    }
    close(network_socket);
 
}

int main()
{
    clienthread();
    return 0;
}