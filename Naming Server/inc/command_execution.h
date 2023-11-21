// command_execution.h
#ifndef COMMAND_EXECUTION_H
#define COMMAND_EXECUTION_H

#include "commands.h"
#include "network.h"
#include "file_operations.h"
#include "defs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"

int executeCmd(command *cmd, int socket, int PERMISSIONS);
int check_SS(id);
int is_directory(const str path);
void send_directory(const str dir_path, int socket);

void send_directory_backup(const str dir_path, int socket, struct HashMap *sent_paths);

#endif // COMMAND_EXECUTION_H

