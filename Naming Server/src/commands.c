// commands.c
#include "../inc/commands.h"

command parser(int socket)
{
    char input[MAX_INPUT_SIZE];
    memset(input, 0 , sizeof(input));
    fgets(input, MAX_INPUT_SIZE, stdin);
    command cmd;
    cmd.client = socket;
    char del[] = " \t\n\v\r\f";
    int n = strlen(input), i = 0;
    strcpy(cmd.cmd, input);
    cmd.cmd[n] = '\0';
    str token = strtok(input, del);
    while(token)
    {
        strcpy(cmd.argv[i++], token);
        token = strtok(NULL, del);
    }
    cmd.argc = i;
    return cmd;
}
