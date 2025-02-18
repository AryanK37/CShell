#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/utsname.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

#include "__my_headers__.h"


const char *getProcessStatus(int pid)
{
    static char status[256];
    FILE *fp;
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    fp = fopen(path, "r");
    if (fp == NULL)
    {
        return "Unknown";
    }

    while (fgets(status, sizeof(status), fp) != NULL)
    {
        if (strncmp(status, "State:", 6) == 0)
        {
            fclose(fp);
            char *newline = strchr(status, '\n');
            if (newline)
            {
                *newline = '\0';
            }
            return status + 7; //"State: " 7 chars
        }
    }
    fclose(fp);
    return "Unknown";
}
int comparePid(const void *a, const void *b)
{
    bgs *processA = (bgs *)a;
    bgs *processB = (bgs *)b;
    return (processA->pid - processB->pid);
}

void printBackgroundProcesses()
{
    qsort(userDetails->myBgs, userDetails->ind, sizeof(bgs), comparePid);
    for (int i = 0; i < userDetails->ind; i++)
    {
        const char *status = getProcessStatus(userDetails->myBgs[i].pid);

        if (strcmp(status, "S (sleeping)") == 0 || strcmp(status, "R (running)") == 0)
        {
            printf("%d : %s - Running\n", userDetails->myBgs[i].pid, userDetails->myBgs[i].name);
        }
        else if (strcmp(status, "T (stopped)") == 0)
        {
            printf("%d : %s - Stopped\n", userDetails->myBgs[i].pid, userDetails->myBgs[i].name);
        }
    }
}
