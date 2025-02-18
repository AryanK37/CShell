#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/utsname.h>

#include "__my_headers__.h"

bool hop(char *path)
{
    // return true;
    // printf("%s\n", path);
    if (path == NULL)
    {
        return false;
    }
    char dir[MAX_SIZE_1];
    if (path[0] == '~')
    {
        if (chdir(userDetails->homeDir) != 0)
        {
            perror(RED"error in changing to home dir"RESET);
            return true;
        }

        // present dir is home dir
        // printf("here\n");
        // last dir is present dir
        strcpy(userDetails->lastDir, userDetails->presentDir);
        getcwd(dir, MAX_SIZE_1);

        strcpy(userDetails->presentDir, dir);
        printf("%s\n", userDetails->presentDir);
        return true;
    }

    else if (strcmp(path, "-") == 0)
    {
        if (chdir(userDetails->lastDir) != 0)
        {
            perror(RED"hop-"RESET);
            return 0;
            // exit(EXIT_FAILURE);
        }

        // swap prsent and last dir
        char temp[MAX_SIZE_1];
        strcpy(temp, userDetails->lastDir);
        strcpy(userDetails->lastDir, userDetails->presentDir);
        strcpy(userDetails->presentDir, temp);

        printf("%s\n", userDetails->presentDir);
        return true;
    }
    int x = chdir(path);
    if (x != 0)
    {
        // printf("%d\n", x);
        perror(RED"hop\n"RESET);
        perror(RED"error in changing dir\n"RESET);
        // printf("helppp: No such file or directory\n");
        return 0;
    }
    else
    {
        getcwd(dir, MAX_SIZE_1);
        strcpy(userDetails->lastDir, userDetails->presentDir);
        strcpy(userDetails->presentDir, dir);
    }
    printf("%s\n", userDetails->presentDir);
    return true;
}