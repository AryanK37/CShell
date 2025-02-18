#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "__my_headers__.h"

void redirectIO(char *file, int flag)
{
    // printf("%s\n", file);
    int fd;
    // printf("Flag value: %d\n", flag);
    if (flag == 1)
    { // >
        fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0)
        {
            perror(RED "Error opening file for output redirection" RESET);
            return;
        }
        dup2(fd, STDOUT_FILENO);
        // printf("hi\n");
    }
    else if (flag == 2)
    { // >>
        fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd < 0)
        {
            perror(RED "Error opening file for append output redirection" RESET);
            return;
        }
        dup2(fd, STDOUT_FILENO);
    }
    else if (flag == 4)
    { // <
        fd = open(file, O_RDONLY);
        if (fd < 0)
        {
            perror(RED "No such input file found!" RESET);
            return;
        }
        dup2(fd, STDIN_FILENO);
    }
    // close(fd);
}

void restoreIO(int stdin_backup, int stdout_backup)
{
    dup2(stdin_backup, STDIN_FILENO);
    dup2(stdout_backup, STDOUT_FILENO);
}