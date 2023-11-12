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

int executeCmd(command *cmd, int socket);

#endif // COMMAND_EXECUTION_H

