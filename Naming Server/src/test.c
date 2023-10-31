#include "../inc/cmds.h"

int main()
{
    command c = parser();
    printf("argc: %d ,len: %d,  cmd: %s", c.argc, strlen(c.cmd), c.cmd);
    int i =0;
    while(i<c.argc)
    {
        printf("arg %d = %s\n", i, c.argv[i]);
        i++;
    }
    return 0;
}