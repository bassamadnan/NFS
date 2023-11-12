// commands.h
#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "defs.h"

#define MAX_INPUT_SIZE 5000
#define MAX_WORDS 512

typedef struct command {
    int client;
    int argc;
    char cmd[MAX_INPUT_SIZE];
    char argv[MAX_WORDS][MAX_INPUT_SIZE];
} command;

command parser(int socket);

#endif // COMMANDS_H

