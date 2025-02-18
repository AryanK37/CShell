#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#include "__my_headers__.h"

typedef struct
{
    char dirName[MAX_SIZE_1];
    char extraInfo[MAX_SIZE_1];
    char col[MAX_SIZE_1];
} stdir;

int compare(const void *a, const void *b)
{
    return strcmp(((stdir *)a)->dirName, ((stdir *)b)->dirName);
}

bool reveal(char *path, int flag)
{
    if (path == NULL) // if no path passed
    {
        return reveal(userDetails->presentDir, flag);
    }

    if (path[0] == '~')
    {
        // Replace '~' with the user's home directory
        char fullPath[4096];
        snprintf(fullPath, sizeof(fullPath), "%s%s", userDetails->homeDir, path + 1);
        return reveal(fullPath, flag);
    }
    else if (strcmp(path, "-") == 0)
    {
        if (strcmp(userDetails->lastDir, "\0") == 0)
        {
            perror(RED "No last Directory\n" RESET);
            return false;
        }

        return reveal(userDetails->lastDir, flag);
    }

    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror(RED "Cant open directory in reveal \n" RESET);
        return false;
    }

    // -l : displays extra information second bit is 1
    // -a : displays all files, including hidden files first bit 1
    struct dirent *de;
    int size = 1000;
    stdir *arr = (stdir *)malloc(size * sizeof(stdir));
    if (arr == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    int total = 0;
    int n = 0;
    while ((de = readdir(dir)) != NULL)
    {
        // Skip hidden files if the first bit of flag is not set
        if (!(flag & 1) && de->d_name[0] == '.')
        {
            continue;
        }
        if ((flag >> 1) & 1) // If the second bit is set, show extra information
        {
            struct stat fileStat;
            char fullPath[4096];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", path, de->d_name);

            if (lstat(fullPath, &fileStat) == -1)
            {
                perror(RED "Cant open stats in Reveal \n" RESET);
                continue;
            }
            // green for executables, white for files and blue for directories].
            if (S_ISDIR(fileStat.st_mode))
            {
                strcpy(arr[n].col, BLUE);
            }
            else if (fileStat.st_mode & S_IXUSR)
            {
                strcpy(arr[n].col, GREEN);
            }
            else
            {
                strcpy(arr[n].col, WHITE);
            }

            char perm_str[4096];
            total += fileStat.st_blocks;
            snprintf(perm_str, sizeof(perm_str), "-%c%c%c%c%c%c%c%c%c %ld %s %s %ld",
                     (fileStat.st_mode & S_IRUSR) ? 'r' : '-',
                     (fileStat.st_mode & S_IWUSR) ? 'w' : '-',
                     (fileStat.st_mode & S_IXUSR) ? 'x' : '-',
                     (fileStat.st_mode & S_IRGRP) ? 'r' : '-',
                     (fileStat.st_mode & S_IWGRP) ? 'w' : '-',
                     (fileStat.st_mode & S_IXGRP) ? 'x' : '-',
                     (fileStat.st_mode & S_IROTH) ? 'r' : '-',
                     (fileStat.st_mode & S_IWOTH) ? 'w' : '-',
                     (fileStat.st_mode & S_IXOTH) ? 'x' : '-',
                     (long)fileStat.st_nlink,
                     (getpwuid(fileStat.st_uid)) ? getpwuid(fileStat.st_uid)->pw_name : "",
                     (getgrgid(fileStat.st_gid)) ? getgrgid(fileStat.st_gid)->gr_name : "",
                     (long)fileStat.st_size);

            time_t modTime = fileStat.st_mtime;
            struct tm *timeInfo = localtime(&modTime);
            char timeStr[80];
            strftime(timeStr, sizeof(timeStr), " %Y-%m-%d %H:%M:%S", timeInfo);
            strcat(perm_str, timeStr);

            strcpy(arr[n].dirName, de->d_name);
            strcpy(arr[n++].extraInfo, perm_str);
        }
        else
        {
            struct stat fileStat;
            char fullPath[4096];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", path, de->d_name);
            if (lstat(fullPath, &fileStat) == -1)
            {
                perror(RED "Cant open stas in Reveal \n" RESET);
                continue;
            }
            // green for executables, white for files and blue for directories].
            if (S_ISDIR(fileStat.st_mode))
            {
                strcpy(arr[n].col, BLUE);
            }
            else if (fileStat.st_mode & S_IXUSR)
            {
                strcpy(arr[n].col, GREEN);
            }
            else
            {
                strcpy(arr[n].col, WHITE);
            }
            arr[n].extraInfo[0] = '\0';
            strcpy(arr[n++].dirName, de->d_name);
        }

        if (n >= size)
        {
            size *= 2;
            arr = (stdir *)realloc(arr, size * sizeof(stdir));
            if (arr == NULL)
            {
                fprintf(stderr, "malloc failed\n");
                closedir(dir);
                return false;
            }
        }
    }

    qsort(arr, n, sizeof(stdir), compare);
    if ((flag & 2))
    {
        printf("Total : %d\n", (total + 1) / 2);
    }

    for (int i = 0; i < n; i++)
    {
        if (flag & (1 << 1))
        {
            printf("%s%s%s ", arr[i].col, arr[i].extraInfo, RESET);
        }
        printf("%s%s%s\n", arr[i].col, arr[i].dirName, RESET);
    }
    free(arr);
    closedir(dir);
    return true;
}