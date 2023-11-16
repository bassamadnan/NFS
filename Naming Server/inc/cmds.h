#include <unistd.h>
#include <pthread.h>
#include "commands.h"
#include "defs.h"
#include "network.h"
#include "file_operations.h"
#include "command_execution.h"

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
    int x = recv(network_socket, response, sizeof(response), 0);
    printf("recieved %d response: \n%s\n", x, response);
}
