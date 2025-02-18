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

void execute_ping(char *pid_str, char *sig_str)
{
    pid_t pid = atoi(pid_str);
    int signal_number = atoi(sig_str) % 32;

    // Send the signal to the process
    if (kill(pid, signal_number) == -1)
    {
        if (errno == ESRCH)
        {
            perror(RED "No such process found\n" RESET);
        }
        else
        {
            perror(RED "Error sending signal" RESET);
        }
    }
    else
    {
        // SIGINT SIGTERM, SIGQUIT and SIGKILL
        // if (signal_number == SIGINT || signal_number == SIGTERM || signal_number == SIGQUIT || signal_number == SIGKILL)
        // {
        //     remove_bg_process(pid);
        // }
        
        printf("Sent signal %d to process with pid %d\n", signal_number, pid);
    }
}



