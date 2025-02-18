#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


#include "__my_headers__.h"

void print_match(const char *path, int is_file)
{
    if (is_file)
    {
        printf("\033[0;32m%s\033[0m\n", path); // Green for files
    }
    else
    {
        printf("\033[0;34m%s/\033[0m\n", path); // Blue for directories
    }
}

void seek(const char *target, const char *directory, int flags, int *count, char *found_path)
{
    // b0 = 1 means d
    // b1 = 1 means f
    // b2 = 1 means e

    DIR *dir = opendir(directory);
    if (!dir)
    {
        perror(RED "Failed to open directory" RESET);
        return;
    }

    struct dirent *entry;
    char path[1024];
    struct stat statbuf;

    int found_files = 0;
    int found_dirs = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);

        if (stat(path, &statbuf) == -1)
        {
            perror("stat failed");
            continue;
        }

        if ((flags & 1) && S_ISDIR(statbuf.st_mode) && strncmp(entry->d_name, target, strlen(target)) == 0)
        {
            print_match(path, 0);
            found_dirs++;
            strcpy(found_path, path);
        }
        else if ((flags & 2) && S_ISREG(statbuf.st_mode) && strncmp(entry->d_name, target, strlen(target)) == 0)
        {
            print_match(path, 1);
            found_files++;
            strcpy(found_path, path);
        }
        else if (!(flags & 1) && !(flags & 2) && strncmp(entry->d_name, target, strlen(target)) == 0)
        {
            if (S_ISDIR(statbuf.st_mode))
            {
                print_match(path, 0);
                found_dirs++;
            }
            else if (S_ISREG(statbuf.st_mode))
            {
                print_match(path, 1);
                found_files++;
            }
            strcpy(found_path, path);
        }

        if (S_ISDIR(statbuf.st_mode))
        {
            seek(target, path, flags, count, found_path);
        }
    }

    closedir(dir);

    *count = found_files + found_dirs; // Update count with the number of found files and directories

    return;
}
