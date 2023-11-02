#include "../inc/cmds.h"
#include "../inc/fileapi.h"
#include "../inc/lru.h"

int main()
{
    str buffer = NULL; str filename = "a.txt";
    size_t filesize;
    readFile(filename, &buffer ,&filesize);
    printf("%d\n%s", filesize, buffer);
    return 0;
}