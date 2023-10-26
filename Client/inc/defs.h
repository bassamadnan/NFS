#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h> // for group
#include <pwd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>  
#include <sys/utsname.h>
#include <sys/wait.h> // wuncrate
#include <time.h>
#include <time.h>
#include <unistd.h>
#include<stdio.h>
#include <arpa/inet.h>
#include <pthread.h>

#define str char*
#define str_arr char**

typedef struct command
{
    int argc;
    str cmd;
    str_arr argv;
}command;

void parser(str input)
{
    // given input , parse it into args and argc
    // command operation -> argv[0] (read xyz/ delete xyz / write xyz)
    // based on argv[0] , use the API for the operation 
    // throw errors !
}