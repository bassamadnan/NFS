/*File related functions*/
#include "../inc/entry.h"


str getBaseName(str filename) { // chatgpt
    const char *lastDot = strrchr(filename, '.'); 

    if (lastDot == NULL) {
        return filename; 
    } else {
        size_t baseLength = (size_t)(lastDot - filename);
        str baseName = (char *)malloc(baseLength + 1);
        strncpy(baseName, filename, baseLength);
        baseName[baseLength] = '\0';
        
        return baseName;
    }
}


// TODO : test this and add all the results in entry.paths (entry struct)

void exploreDirectory(str path) // seek code from MP1 - bassamadnan
{ 
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Unable to open directory");
        return;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (cmp(entry->d_name, ".")|| cmp(entry->d_name, "..")) {
            continue;
        }
        str temp = calloc(MAX_NAME_SIZE, sizeof(char));
        temp = getBaseName(entry->d_name);
        // printf("%s\n", temp);
        // if(flagf && cmp(temp, targetName) && !(cmp(temp, entry->d_name))){
        //     char tp[MAX_PATH_SIZE];
        //     snprintf(tp, sizeof(tp), "%s/%s", path, entry->d_name);
        //     reccall++;
        //     green(tp);
        // }
        char newEntryPath[MAX_PATH_SIZE];
        snprintf(newEntryPath, sizeof(newEntryPath), "%s/%s", path, entry->d_name);
        struct stat st = {0};
        stat(entry->d_name, &st);
        mode_t mode = st.st_mode;
        if (entry->d_type == DT_DIR) {
            exploreDirectory(newEntryPath);
        }
    }
    
    closedir(dir);
}