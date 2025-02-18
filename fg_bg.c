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

int is_valid_pid(const char *str)
{
    if (str == NULL || *str == '\0')
    {
        return 0;
    }
    for (int i = 0; str[i]; i++)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }
    char proc_path[256];
    snprintf(proc_path, sizeof(proc_path), "/proc/%s", str);
    struct stat sb;
    if (stat(proc_path, &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        return 1;
    }
    return 0;
}

void fg(char *pid_str)
{
    if (!is_valid_pid(pid_str))
    {
        perror(RED "Error: Invalid or empty PID string.\n" RESET);
        return;
    }

    pid_t pid = (pid_t)atoi(pid_str);
    if (pid <= 0)
    {
        perror(RED "Error: Invalid PID value.\n" RESET);
        return;
    }
    // remove_bg_process(pid);

    int fore_pgid = getpgrp();

    // writes to stdout, and since its stdout is not redirected, it writes to the terminal
    // so i am sending sigttou signal to stop bg process from writing to terminal

    if (signal(SIGTTOU, SIG_IGN) == SIG_ERR)
    {
        perror(RED "Error: Failed to ignore SIGTTOU signal." RESET);
        return;
    }

    

    pid_t ok = getpgid(pid);
    if (tcsetpgrp(STDIN_FILENO, ok) == -1)
    {
        perror("Error setting process group to foreground");
        return;
    }
    kill(-pid, SIGCONT); // all processes to continue

    int status;
    if (waitpid(pid, &status, WUNTRACED) == -1)
    {
        perror(RED "Error waiting for process" RESET);
        return;
    }
    tcsetpgrp(STDIN_FILENO, fore_pgid);

    // Once the process stops or terminates
    // return the terminal control back
    if (tcsetpgrp(STDIN_FILENO, getpgrp()) == -1)
    {
        perror("Error returning terminal control to shell");
        return;
    }

    // return the previous signal mask
    if (signal(SIGTTOU, SIG_DFL) == SIG_ERR)
    {
        perror(RED "Error: Failed to restore default SIGTTOU handling." RESET);
        return;
    }

    if (WIFSTOPPED(status))
    {
        printf("Process %d stopped by signal %d\n", pid, WSTOPSIG(status));
    }
    else if (WIFEXITED(status))
    {
        printf("Process %d exited with status %d\n", pid, WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        printf("Process %d was killed by signal %d\n", pid, WTERMSIG(status));
    }
}

void bg_exec(char *pid_str)
{
    pid_t pid = atoi(pid_str);
    // ping();
    kill(-pid, SIGCONT);
}
