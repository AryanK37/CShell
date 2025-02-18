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

#define MAX_TOKENS 100

void execCmd_IO(char *cmd, int hasAnd)
{
    int stdin_backup = dup(STDIN_FILENO);
    int stdout_backup = dup(STDOUT_FILENO);
    // for (int i = 0; i < n; i++)
    {
        if (cmd[strlen(cmd) - 1] == '&')
        {
            hasAnd = 1;
            cmd[strlen(cmd) - 1] = '\0';
        }
        int rdirect_in = 0, rdirect_out = 0;
        char *input_file = NULL;
        char *output_file = NULL;
        int append = 0;
        char copycmd[MAX_SIZE_1];
        char execmd[MAX_SIZE_1];
        strcpy(copycmd, cmd);
        execmd[0] = '\0';

        // if (cmd[strlen(cmd) - 1] == '&')
        // {
        //     hasAnd = 1;
        //     cmd[strlen(cmd) - 1] = '\0';
        // }

        // Handling output redirection
        if (strstr(cmd, ">"))
        {
            char *out_ptr = strstr(cmd, ">");
            if (*(out_ptr + 1) == '>')
            {
                append = 1;
                *out_ptr++ = '\0'; // Null terminate at first '>'
            }
            *out_ptr++ = '\0'; // Null terminate at first '>'
            output_file = strtok(out_ptr, " ");
            rdirect_out = 1;
        }

        // Handling input redirection
        if (strstr(cmd, "<"))
        {
            char *in_ptr = strstr(cmd, "<");
            *in_ptr++ = '\0'; // Null terminate at first '<'
            input_file = strtok(in_ptr, " ");
            rdirect_in = 1;
        }

        // Get the executable command part
        if (execmd[0] == '\0')
        {
            strcpy(execmd, cmd);
        }

        // Apply input redirection if needed
        if (rdirect_in)
        {
            redirectIO(input_file, 4);
        }

        // Apply output redirection if needed
        if (rdirect_out)
        {
            int flag = append ? 2 : 1;
            redirectIO(output_file, flag);
        }

        // Execute the command
        // execCmd(execmd, hasAnd);
        execCmd(execmd, hasAnd);

        // Restore standard input/output after command execution
        restoreIO(stdin_backup, stdout_backup);
    }
}

void check_pipe_execute(char *execmd, int hasAnd)
{
    
    char *commands[MAX_TOKENS];
    int num_commands = 0;
    // cat newfile.txt | wc |
    int len = strlen(execmd);
    while (len > 0 && isspace(execmd[len - 1]))
    {
        len--;
    }

    if (len > 0 && execmd[len - 1] == '|')
    {
        fprintf(stderr, RED "Error: Invalid command, trailing pipe detected.\n" RESET);
        return;
    }

    char *token = strtok(execmd, "|");
    while (token != NULL)
    {
        if (strlen(token) == 0)
        {
            perror(RED "Error: Invalid command, empty command detected near pipe.\n" RESET);
            return;
        }
        commands[num_commands++] = token;
        token = strtok(NULL, "|");
    }
    commands[num_commands] = NULL; // Null-terminate the array

    // If there's no pipe, execute the command directly
    if (num_commands == 1)
    {
        execCmd_IO(commands[0], hasAnd);
        return;
    }

    // Handle piping
    int fd[2];
    int fd_in = 0; // Input file descriptor for the first command is standard input

    for (int i = 0; i < num_commands; i++)
    {
        pipe(fd); // Create a pipe

        pid_t pid = fork(); // Fork a child process
        if (pid == 0)
        {
            // Child process
            dup2(fd_in, STDIN_FILENO); // Replace standard input with the input file descriptor

            if (commands[i + 1] != NULL)
            {
                dup2(fd[1], STDOUT_FILENO); // Replace standard output with the write end of the pipe
            }

            close(fd[0]); // Close unused read end of the pipe
            // execute_command(commands[i]); // Execute the current command
            execCmd_IO(commands[i], hasAnd);
            exit(0); // Exit child process
        }
        else if (pid > 0)
        {
            // Parent process
            wait(NULL);    // Wait for the child process to complete
            close(fd[1]);  // Close the write end of the pipe
            fd_in = fd[0]; // Save the read end of the pipe for the next command
        }
        else
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }
    // close(fd[1]);  // Properly close the write end of the pipe in the parent process

}