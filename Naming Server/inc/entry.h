#ifndef ENTRY_H
#define ENTRY_H

typedef struct entry
{
    int id;
    char ip[IP_SIZE];
    int cport;
    int nmport;
    int entries;
    char paths[MAX_ENTRIES][MAX_PATH_SIZE];
}entry;


#endif
