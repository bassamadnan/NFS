#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // Command and arguments for ls
    char *command = "ls";
    char *args[] = {"ls", NULL};

    // Call execvp
    if (execvp(command, args) == -1) {
        perror("execvp");
        exit(EXIT_FAILURE);
    }

    // This code is reached only if execvp fails
    fprintf(stderr, "Error: execvp should not return\n");
    exit(EXIT_FAILURE);

    return 0;
}

