#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h> // for group
#include <pwd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>  
#include <sys/utsname.h>
#include <sys/wait.h> // wuncrate
#include <time.h>
#include <time.h>
#include <unistd.h>
#include<stdio.h>
#include <arpa/inet.h>
#include <pthread.h>

void* clienthread()
{
 
    // int client_request = *((int*)args);
    // scanf("%d", &1);
    char client_request[] = "a.txt";
    int network_socket;
 
    // Create a stream socket
    network_socket = socket(AF_INET,
                            SOCK_STREAM, 0);
 
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8989);
 
    // Initiate a socket connection
    int connection_status = connect(network_socket,
                                    (struct sockaddr*)&server_address,
                                    sizeof(server_address));
 
    // Check for connection error
    if (connection_status < 0) {
        puts("Error\n");
        return 0;
    }
 
    // printf("Connection established\n");
    // Send data to the socket
    send(network_socket, &client_request,
         sizeof(client_request), 0);
 
    char reply[4096];
    recv(network_socket, reply, sizeof(reply), 0);
    printf("%s\n", reply);
    // Close the connection
    close(network_socket);
 
}

int main()
{
    clienthread();
    // while(1)
    // {

    // }
    // sleep(5);
    // printf("connectin close\n");
    return 0;
}