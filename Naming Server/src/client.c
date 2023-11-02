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
    for(;;)
    {
        command c = parser();
        send(network_socket, PARAMS(c));
        sleep(1);
        // entry e;
        // recv(network_socket, PARAMS(e));
        // if(e.id != -1)
        // printf("Got back SS ID: %d, ss cport: %d , ss ip: %s\n", e.id, e.cport, e.ip);
        // else
        // printf("Not found\n");

    }
    close(network_socket);
 
}

int main()
{
    clienthread();
    return 0;
}