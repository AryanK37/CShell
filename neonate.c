#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>

#include "__my_headers__.h"

int get_recent_pid()
{
    FILE *fp = fopen("/proc/loadavg", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    float load1, load5, load15;
    int running_tasks, total_tasks, recent_pid;
    fscanf(fp, "%f %f %f %d/%d %d", &load1, &load5, &load15, &running_tasks, &total_tasks, &recent_pid);
    return recent_pid;
}

void enter_raw_mode(struct termios *orig_termios)
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, orig_termios);
    raw = *orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
void disable_raw_mode(struct termios *orig_termios)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios);
}

void neonate(int n)
{
    if (n <= 0)
    {
        fprintf(stderr, "Invalid time argument: %d\n", n);
        return;
    }
    pid_t pid = fork();
    if (pid < 0)
    {
        perror(RED "Fork failed" RESET);
        return;
    }
    if (pid == 0)
    {

        while (1)
        {
            int recent_pid = get_recent_pid();
            if (recent_pid != -1)
            {
                printf("Recent PID: %d\n", recent_pid);
            }
            else
            {
                printf("Error getting recent PID\n");
            }
            sleep(n);
        }
        return;
    }
    else
    {
        struct termios orig_termios;
        enter_raw_mode(&orig_termios);
        while (1)
        {
            char c = getchar();
            if (c == 'x')
            {
                kill(pid, SIGTERM);
                break;
            }
        }
        disable_raw_mode(&orig_termios);
    }
}
