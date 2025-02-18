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

#include "__my_headers__.h"

#define MAX_SIZE_2 4096

#define MAX_TOKENS 100

User_Details *userDetails;

void handle_signal(int signum)
{
    if (signum == SIGTSTP)
    {
        // printf("%d here1\n", userDetails->forRunn);
        if (userDetails->forRunn != -1)
        {
            // printf("%d here2\n", userDetails->forRunn);
            setpgid(userDetails->forRunn, 0);
            // kill(-fg_pid, SIGTSTP);
            kill(userDetails->forRunn, SIGTSTP);
            userDetails->myBgs[userDetails->ind].pid = userDetails->forRunn;
            strcpy(userDetails->myBgs[userDetails->ind].name, userDetails->sig);
            userDetails->ind++;
            userDetails->forRunn = -1;
            return;
        }
        else
        {
            printf("\n");
            printPrompt();
            fflush(stdout);
        }
        return;
    }
    else if (signum == SIGINT)
    {
        if (userDetails->forRunn != -1)
        {
            remove(userDetails->forRunn);
            kill(userDetails->forRunn, SIGINT);
            userDetails->forRunn = -1;
            return;
        }
        else
        {
            printf("\n");
            printPrompt();
            fflush(stdout);
        }
    }
}
// activities done but status
// bg add done
// ctrl z add done
// fg remove done
// kill remove SIGINT SIGTERM, SIGQUIT and SIGKILL
// ping
// bg change status not needed

// implement bg

int main()
{
    alias();
    struct sigaction sa;

    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_flags = SA_RESTART; // read write are not interrupted by signals

    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    // signal(SIGTSTP, handle_signal);
    // signal(SIGINT, handle_signal);

    // signal(SIGTSTP,handl)

    userDetails = malloc(sizeof(User_Details));
    userDetails->ind = 0;
    userDetails->forRunn = -1;
    getUserDetails();
    strcpy(userDetails->homeDir, userDetails->invokedDir);
    userDetails->lastDir[0] = '\0';
    while (1)
    {
        // printf("%d\n", userDetails->ind);
        fflush(stdin);
        printPrompt();
        userDetails->forRunn = -1;
        char *ucmd = malloc(MAX_SIZE_1);
        getCommand(ucmd);

        if (strlen(ucmd) >= 3)
        {
            if ((ucmd[0] != 'l' || ucmd[1] != 'o' || ucmd[2] != 'g'))
            {
                log_write(ucmd);
            }
        }
        else
        {
            log_write(ucmd);
        }
        fflush(stdin);
        tokenize_execute(ucmd);
    }
    return 0;
}