#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/utsname.h>
#include <sys/wait.h>

#include "__my_headers__.h"

void getUserDetails()
{
    char *user = getenv("USER");
    if (user == NULL)
    {
        perror(RED "\1" RESET);
        exit(EXIT_FAILURE);
    }
    strncpy(userDetails->userName, user, sizeof(userDetails->userName));
    userDetails->userName[sizeof(userDetails->userName) - 1] = '\0';

    struct utsname sysInfo;
    if (uname(&sysInfo) < 0)
    {
        perror(RED "\1" RESET);
        exit(EXIT_FAILURE);
    }

    strncpy(userDetails->systemName, sysInfo.nodename, sizeof(userDetails->systemName));
    userDetails->systemName[sizeof(userDetails->systemName) - 1] = '\0'; // Ensure null-termination

    if (getcwd(userDetails->invokedDir, sizeof(userDetails->invokedDir)) == NULL)
    {
        perror(RED "\1" RESET);
        exit(EXIT_FAILURE);
    }
    return;
}

void printPrompt()
{
    // check if invoked directory is substring of present directory if it is then print replacing it with /home else printing complete path
    char currentDir[256];
    if (getcwd(currentDir, sizeof(currentDir)) == NULL)
    {
        perror(RED "\1" RESET);
        exit(EXIT_FAILURE);
    }

    // strcpy(userDetails->homeDir, currentDir);
    strcpy(userDetails->presentDir, currentDir);

    // Checking if the invoked directory is a substring of the current directory
    char *position = strstr(currentDir, userDetails->invokedDir);

    int result = 0;
    int status;
    result = waitpid(-1, &status, WNOHANG);
    // printf("%d\n",result);
    if (result > 0)
    {
        // , WEXITSTATUS(status)
        if (WIFEXITED(status))
        {
            printf("Child process (PID: %d) exited with status Normally.\n", result);
        }
        else
        {
            // printf("haha %d\n", WIFEXITED(status));
            printf("Child process (PID: %d) did not exit normally.\n", result);
        }
    }

    if (position != NULL && position == currentDir)
    {
        // replacing with "/~"
        printf("<%s%s%s@%s%s%s:/~%s %s>", BOLD_GREEN, userDetails->userName, RESET, BOLD_BLUE, userDetails->systemName, RESET, currentDir + strlen(userDetails->invokedDir), userDetails->sig);
        // printf("%s%s@%s%s%s %s%s%s %s%s%s %s\n",
        //        BG_BLUE, FG_BLACK, userDetails->userName, FG_DEFAULT, userDetails->systemName,
        //        RESET, BG_GREEN, FG_BLUE, userDetails->sig, FG_BLACK, currentDir + strlen(userDetails->invokedDir),
        //        BG_DEFAULT, userDetails->sig, RESET);
    }
    else
    {
        // full current directory
        printf("<%s%s%s@%s%s%s:%s %s>", BOLD_GREEN, userDetails->userName, RESET, BOLD_BLUE, userDetails->systemName, RESET, currentDir, userDetails->sig);
        // printf("%s%s@%s%s:%s%s %s%s%s%s\n",
        //        BG_BLUE, FG_BLACK, userDetails->userName, FG_DEFAULT, userDetails->systemName, RESET,
        //        BG_GREEN, FG_BLUE, currentDir, RESET);
        // printf("%s%s@%s%s%s %s%s%s %s%s%s %s\n",
        //        BG_BLUE, FG_BLACK, userDetails->userName, FG_DEFAULT, userDetails->systemName,
        //        RESET, BG_GREEN, FG_BLUE, userDetails->sig, FG_BLACK, currentDir,
        //        BG_DEFAULT, userDetails->sig, RESET);
    }
    userDetails->sig[0] = '\0';

    fflush(stdout);
    return;
}

void getCommand(char *cmd)
{
    if (fgets(cmd, MAX_SIZE_1, stdin) == NULL)
    {
        // printf("HERE I am in sigquit\n");
        for (int i = 0; i < userDetails->ind; i++)
        {
            // printf("HERE %d\n", userDetails->myBgs[i].pid);
            kill(userDetails->myBgs[i].pid, SIGTERM);
        }
        fflush(stdout);
        exit(0);
    }
    size_t len = strlen(cmd);
    if (len > 0 && cmd[len - 1] == '\n')
    {
        cmd[len - 1] = '\0';
    }
    return;
}