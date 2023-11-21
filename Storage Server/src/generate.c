#include "../inc/entry.h"
/*
Generate a txt of files containing all paths and directories relative to current directroy
*/

const char *output_filename = "generate.txt";
FILE *output_file;
char current_directory[MAX_PATH_SIZE];

int is_directory(const str path) {
    DIR *directory = opendir(path);
    if (directory) {
        closedir(directory);
        return 1;
    }
    return 0;
}

void trim_path(const str path)
{
    int i=0;
    while(i< strlen(current_directory) && current_directory[i] == *path)
    {
        path++;
        i++;
    }
    path++;
    fprintf(output_file, "%s\n", path);

}

void list_relative_paths(const str dir_path, FILE *file) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "Failed to open directory: %s\n", dir_path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;  // Skip current directory and parent directory entries.
        }

        char full_path[MAX_PATH_SIZE];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        if (is_directory(full_path)) {
            trim_path(full_path);
            list_relative_paths(full_path, file);  // Recursive call for subdirectories.
        } else {
            trim_path(full_path);
        }
    }

    closedir(dir);
}

int main() {
    output_file = fopen(output_filename, "w");
    if (!output_file) {
        fprintf(stderr, "Failed to open file for writing: %s\n", output_filename);
        return 1;
    }
    if (getcwd(current_directory, sizeof(current_directory)) == NULL) {
        fprintf(stderr, "Failed to get current working directory\n");
        fclose(output_file);
        return 1;
    }

    list_relative_paths(current_directory, output_file);
    fclose(output_file);

    printf("Relative paths have been written to %s\n", output_filename);

    return 0;
}