#include "../inc/cmds.h"

#define SRC 1024
void print(int client_socket, str path) {

    FILE *file;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    file = fopen(path, "r");
    if (file == NULL) {
        perror("File open error");
        exit(1);
    }
    int x= 0;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        // if (send(client_socket, buffer, bytes_read, 0) == -1) {
        //     perror("Send error");
        //     exit(1);
        x += bytes_read;
        printf("Sent buffer: %s x\n", buffer);
    }
    printf("%d bytes sent\n", x);
    fclose(file);
}

int main()
{
    str buffer = NULL; str filename = "a.txt";
    size_t filesize;
    print(0, "a.txt");
    return 0;
}