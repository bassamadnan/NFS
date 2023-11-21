/*
Retreieve all the information of a file
Various codes have been taken from sehll (MP1)
*/

#include "../inc/entry.h"

void get_permissions(mode_t mode){ // stacke overflow

    char s[32]; int i = 0;
    //for the file type
    if (S_ISREG(mode))
        s[i]= '-';
    else if (S_ISDIR(mode))
        s[i] = 'd';
    else if (S_ISCHR(mode))
        s[i] = 'c';
    else if (S_ISBLK(mode))
        s[i] = 'b';
    else if (S_ISFIFO(mode))
        s[i] = 'f';
    else if (S_ISLNK(mode))
        s[i] = 'l';
    else
        s[i] = 's';

    i++;
    //for all the permissions
    s[i++] = (mode & S_IRUSR) ? 'r' : '-';
    s[i++] = (mode & S_IWUSR) ? 'w' : '-';
    s[i++] = (mode & S_IXUSR) ? 'x' : '-';
    s[i++] = (mode & S_IRGRP) ? 'r' : '-';
    s[i++] = (mode & S_IWGRP) ? 'w' : '-';
    s[i++] = (mode & S_IXGRP) ? 'x' : '-';
    s[i++] = (mode & S_IROTH) ? 'r' : '-';
    s[i++] = (mode & S_IWOTH) ? 'w' : '-';
    s[i++] = (mode & S_IXOTH) ? 'x' : '-';
    s[i++] = ' ';
    s[i++] = '\0';
    printf("%s", s);
    
}

void display_total(str path){
    struct stat filest;
    stat(path, &filest);
    // TODO: always shows 8?
    printf("total %d\n", (int) filest.st_blocks);

}

void list()
{
    struct dirent **namelist;
    n = scandir(".", &namelist, NULL, alphasort); //chatgpt
    display_total();

    if (n < 0)
        perror("scandir");
    else {
        for (int i = 0; i < n; i++) {
            struct stat st = {0};
            char date[32];
            stat(namelist[i]->d_name, &st);
            mode_t mode = st.st_mode;
            get_permissions(mode);
            struct passwd *pw = getpwuid(st.st_uid);  // error check for pw, gr = 0
            struct group *gr = getgrgid(st.st_gid); 
            if(pw == NULL || gr == NULL){
                printf("error: error fetching info for %s\n", namelist[i]->d_name);
                free(namelist[i]);
                continue;
            }
            printf("%d %s %s %jd ", (int) st.st_nlink, pw->pw_name, gr->gr_name, (intmax_t) st.st_size);
            strftime(date, 32, "%b %2d %2H:%2M", localtime(&(st.st_mtime)));
            printf("%s ", date);
            if(is_dir(mode)){
                blue(namelist[i]-> d_name);
            }else if(is_exe(mode)){
                green(namelist[i]-> d_name);
            }
            else 
                printf("%s\n", namelist[i]->d_name);
            free(namelist[i]);
            }
        }
    free(namelist);
}