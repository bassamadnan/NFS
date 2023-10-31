#include "../inc/cmds.h"

void access_path(entry * e)
{

    FILE* fp = fopen("generate.txt", "r");
    int i = 0;   
    if (fp == NULL) {
        perror("Error opening the file");
        return;
    }

    char line[256]; // Assumes each line is not longer than 255 characters
    while (fgets(line, sizeof(line), fp) != NULL) {
        strcpy((*e).paths[i++], line);
    }
    (*e).entries = i;
    fclose(fp);
}
void* server_thread()
{
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
 
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(6060);
    int connection_status;
    if(connection(&network_socket, &server_address, &connection_status))
    {
        printf("Error in connection\n"); return 0;
    }
    entry e;
    e.cport = 52;
    strcpy(e.ip, "0000.0000.0000");
    e.nmport = 55;
    access_path(&e);
    int i =0;
    while(i<e.entries) printf("%s", e.paths[i++]);
    send(network_socket, &e, sizeof(e), 0);
    close(network_socket);
 
}

void init()
{   
    // initialize a connection to the NM (NM listens at port 6060 for SS)
    // read all paths from generate.txt, send it to NM

    return;
}

int main()
{
    // every server will have one thread for handling connection with NM,
    // every server will have one thread for handling connectionS with clients
    server_thread();
    // while(1)
    // {

    // }
    sleep(5);
    // printf("connectin close\n");
    return 0;
}