#include "../inc/cmds.h"

#define SRC 1024

int main()
{
    sem_init(&clientLock, 0 , 10);
    str buffer = NULL; str filename = "a.txt";
    size_t filesize;
    readFile(filename, &buffer ,&filesize);
    printf("%ld\n%s", filesize, buffer);
    return 0;
}