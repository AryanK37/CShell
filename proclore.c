#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "__my_headers__.h"

void proclore(char *pid)
{
    char path[256];
    char buffer[1024];
    char status_file[MAX_SIZE_1];
    char exe_path[MAX_SIZE_1];
    char line[256];

    if (pid == NULL)
    {
        snprintf(path, sizeof(path), "/proc/self");
    }
    else
    {
        snprintf(path, sizeof(path), "/proc/%s", pid);
    }

    snprintf(status_file, sizeof(status_file), "%s/status", path);
    FILE *status_fp = fopen(status_file, "r");
    if (!status_fp)
    {
        perror("Failed to open status file");
        return;
    }

    char *proc_status = NULL;
    int proc_group = 0;
    int session_id = 0;
    long virtual_memory = 0;

    while (fgets(line, sizeof(line), status_fp))
    {
        if (strncmp(line, "Pid:", 4) == 0)
        {
            printf("pid : %s", line + 5);
        }
        else if (strncmp(line, "State:", 6) == 0)
        {
            proc_status = strdup(line + 7); // Copy process status
        }
        else if (strncmp(line, "Tgid:", 5) == 0)
        {
            proc_group = atoi(line + 6);
        }
        else if (strncmp(line, "VmSize:", 7) == 0)
        {
            virtual_memory = atol(line + 8);
        }
        else if (strncmp(line, "Sid:", 4) == 0)
        {
            session_id = atoi(line + 5);
        }
    }
    fclose(status_fp);

    pid_t current_pgrp = getpgrp();
    // printf("Current process group: %d\n", current_pgrp);
    // if (current_pgrp == proc_group)
    // {
    //     printf("This process is in the same process group as the current process.\n");
    // }
    // else
    // {
    //     printf("This process is NOT in the same process group as the current process.\n");
    // }

    // Determine if the process is in the foreground or background
    pid_t foreground_pgid = tcgetpgrp(STDOUT_FILENO);
    int fore = 0;
    if (foreground_pgid == -1)
    {
        perror(RED "Failed to get foreground process group" RESET);
    }
    else
    {
        // printf("Foreground process group: %d\n", foreground_pgid);
        if (foreground_pgid == proc_group)
        if (foreground_pgid == current_pgrp)
        // if(userDetails->forRunn == -1|| userDetails->forRunn==current_pgrp)
        {
            fore = 1;
            // printf("This process is running in the foreground.\n");
        }
        else
        {
            fore = 0;
            // printf("This process is running in the background.\n");
        }
    }
    if (proc_status)
    {
        if (fore)
        {
            printf("why ra\n");
            printf("fore : %d , cur : %d \n",foreground_pgid , current_pgrp);
            printf("process status : %c+\n", proc_status[0]);
        }
        else
        {
            printf("process status : %c\n", proc_status[0]);
        }
        // printf("process status : %c\n", proc_status[0]);
        free(proc_status);
    }
    printf("Process Group : %d\n", proc_group);
    printf("Session ID : %d\n", session_id);
    printf("Virtual memory : %ld kB\n", virtual_memory);

    snprintf(exe_path, sizeof(exe_path), "%s/exe", path);
    ssize_t len = readlink(exe_path, buffer, sizeof(buffer) - 1);
    if (len != -1)
    {
        buffer[len] = '\0';
        printf("executable path : %s\n", buffer);
    }
    else
    {
        perror("Failed to read executable path");
    }
    return;
}
