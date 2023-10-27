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

#define IP_SIZE     16
#define MAX_PATH_SIZE 4026
#define MAX_NAME_SIZE   1024
#define MAX_ENTRIES     1024
#define str     char*
#define str_arr char**

typedef struct entry
{
    char ip[IP_SIZE];
    int nmport;
    int cport;
    str_arr paths;
}entry;