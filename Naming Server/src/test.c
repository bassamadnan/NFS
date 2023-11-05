#include "../inc/cmds.h"
#include "../inc/fileapi.h"
#include "../inc/lru.h"
#define SRC 1024

int main()
{
    str buffer = NULL; str filename = "a.txt";
    size_t filesize;
    readFile(filename, &buffer ,&filesize);
    printf("%ld\n%s", filesize, buffer);
    return 0;
}