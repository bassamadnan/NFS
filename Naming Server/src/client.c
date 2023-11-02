#include "../inc/cmds.h"
int SS_connect(entry e, command * c)
{
    // given an entry e, connect to cport and e.ip 
    printf("Recieved copy %s\n", c->argv[0]);
    command copy = *c;
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons((uint16_t)e.cport); // client port of this entry
    int connection_status;
    if(connection(&network_socket, &server_address, &connection_status))
    {
        printf("Error in connection\n"); return 0;
    }
    // send(network_socket, PARAMS(copy));
    // char reply[MAX_INPUT_SIZE];
    // recv(network_socket, PARAMS(reply));
    // printf("%s", reply);
    return 0;
}
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
    // for(;;)
    // {
        printf("Reached in thread\n");
        command c;
        parser(&c);
        printf("Recieved %s \n", c.argv[0]);
        // send(network_socket, PARAMS(c));
        // entry e;
        // sleep(10);
        // recv(network_socket, PARAMS(e));
        // if(e.id != -1)
        // printf("Got back SS ID: %d, ss cport: %d , ss ip: %s\n", e.id, e.cport, e.ip);
        // else
        // printf("Not found\n");

    // }
    close(network_socket);
    entry e;
    e.id = 0;
    e.cport = 6061;
    strcpy(e.init_path, "init_path");
    strcpy(e.ip, "000.000.010.000");
    e.nmport = NM_SERVER_PORT;  // communication to the NM via the 6060 port
    printf("Sending connection request..\n");
    SS_connect(e, &c);
 
}

int main()
{
    clienthread();
    return 0;
}