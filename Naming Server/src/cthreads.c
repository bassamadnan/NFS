/* Handles client thread functions for the  NM*/
// #include "../inc/cmds.h"

int check_path(str path)
{
    // returns ID if the path is present in the NM  with its SS ID
    // returns -1 if path is not found
    return 0; 
}

int SS_connect(entry e, command c)
{
    // given an entry e, connect to cport and e.ip 
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(e.cport); // client port of this entry
    int connection_status;
    if(connection(&network_socket, &server_address, &connection_status))
    {
        printf("Error in connection\n"); return 0;
    }
    send(network_socket, PARAMS(c));
    char reply[MAX_INPUT_SIZE];
    recv(network_socket, PARAMS(reply));
    printf("%s", reply);
    return 0;
}