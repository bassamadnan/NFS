#include "../inc/cmds.h"


void* clienthread()
{
 
    // int client_request = *((int*)args);
    // scanf("%d", &1);
    // char client_request[] = "a.txt";
    int network_socket;
 
    // Create a stream socket
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
 
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(5050);
    int connection_status;
    // Initiate a socket connection
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
        msgsend(&network_socket, client_request);
        char reply[4096];
        recv(network_socket, reply, sizeof(reply), 0);
        printf("%s\n", reply);
        fflush(stdout);

    }
    close(network_socket);
 
}

int main()
{
    clienthread();
    // while(1)
    // {

    // }
    sleep(5);
    // printf("connectin close\n");
    return 0;
}