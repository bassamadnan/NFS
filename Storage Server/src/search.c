#include "../inc/entry.h"
/*Code related to optimized search of files*/

void find(str path)
{
    return;
}
int compare_strings(const void* s1, const void* s2) {
    return strcmp(*(const str_arr) s1, *(const str_arr) s2);
}

str_arr get_paths()
{
    // returns accesible paths 
    // from generate.txt (by default contains all relative files and folders) in soretd manner
    FILE *fp = fopen("generate.txt", "r");
    if(!fp)
    {
        printf("ERROR\n");
        return NULL;
    }

    char line[MAX_PATH_SIZE];
    int i =0;
    str_arr accesible_paths = calloc(MAX_ENTRIES, sizeof(str));

    while(fgets(line, sizeof(line), fp) != NULL)
    {
        accesible_paths[i] = calloc(MAX_PATH_SIZE, sizeof(char));
        strcpy(accesible_paths[i++], line);
    }
    qsort(accesible_paths, i, sizeof(char*), compare_strings);
    fclose(fp);
    return accesible_paths;
}

void add_entry()
{
    return;
}

int main(){

    str_arr test = get_paths();
    int i =0;
    while(test[i]) printf("%s", test[i++]);
    return 0;
}