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

#define ISFILE(mode) S_ISREG(mode)
#define ISDIR(mode) S_ISDIR(mode)

#include "__my_headers__.h"

#define MAX_SIZE_2 4096

#define MAX_TOKENS 100

int execCmd(char *cmd, int bg)
{
	char cpcmd[MAX_SIZE_1];
	strcpy(cpcmd, cmd);
	char **tokens = (char **)malloc((MAX_TOKENS) * sizeof(char *));
	int i = 0;
	char *start = cmd;
	int in_quotes = 0;
	char quote_char = '\0';

	while (*cmd)
	{
		// Skip leading spaces
		while (isspace((unsigned char)*cmd))
			cmd++;

		if (*cmd == '\0')
			break; // End of input

		if (*cmd == '"' || *cmd == '\'')
		{
			// Start of a quoted token
			in_quotes = 1;
			quote_char = *cmd;
			start = ++cmd;
		}
		else
		{
			// Start of a normal token
			start = cmd;
		}

		if (in_quotes)
		{
			// Read until the matching quote is found
			while (*cmd && *cmd != quote_char)
				cmd++;
			if (*cmd == quote_char)
			{
				*cmd++ = '\0'; // Null-terminate the quoted token
			}
			in_quotes = 0;
		}
		else
		{
			// Read until the next space or end of input
			while (*cmd && !isspace((unsigned char)*cmd))
				cmd++;
			if (*cmd)
				*cmd++ = '\0'; // Null-terminate the normal token
		}

		// Save the token
		tokens[i++] = start;
	}

	tokens[i] = NULL; // Null-terminate the token array
	int n = i;		  // Set the number of tokens

	if (n == 0)
	{
		return 1;
		// printf("some problem in execCmd\n");
		// exit(EXIT_FAILURE);
	}
	for (int i = 1; i <= n; i++)
	{
		if (strcmp(tokens[0], "hop") == 0)
		{
			if (tokens[1] == NULL)
			{
				// no argument
				hop(userDetails->homeDir);
			}
			else
			{
				while (tokens[i] != NULL)
				{
					// printf("called %s\n",tokens[i]);
					hop(tokens[i]);
					i++;
				}
			}
			return 1;
		}
		else if (strcmp(tokens[0], "log") == 0)
		{
			// printf("yhn to aaya hu ma\n");
			if (strcmp(tokens[n - 1], "log") == 0) // no flag
			{
				log_print();
				return true;
			}
			if (strcmp(tokens[1], "purge") == 0) // purge flag
			{
				log_purge();
				return true;
			}
			if (strcmp(tokens[1], "execute") == 0)
			{
				char *dmd = malloc(MAX_SIZE_1);
				// printf("%s\n", dmd);
				log_exec(atoi(tokens[2]), dmd);
				tokenize_execute(dmd); // need change here to handle multople commands ; separated
				return true;
			}
			return 1;
		}
		else if (strcmp(tokens[0], "reveal") == 0)
		{
			int flag = 0;
			char path[MAX_SIZE_1];
			// printf("%s\n", tokens[n - 1]);
			// HANDLE if path name starts with -
			if (strcmp(tokens[n - 1], "reveal") == 0)
			{
				reveal(userDetails->presentDir, flag);
				return 1;
			}
			while (tokens[i] != NULL)
			{
				int fc = flag;
				if (tokens[i][0] == '-') // is a token
				{
					for (int j = 0; j < strlen(tokens[i]); j++)
					{
						if (tokens[i][j] == 'a')
						{
							flag |= 1;
						}
						else if (tokens[i][j] == 'l')
						{
							flag |= 2;
						}
						else if (tokens[i][j] != 'a' && tokens[i][j] != '-' && tokens[i][j] != 'l' && tokens[i][j] != '\0')
						{
							flag = fc;
							i++;
							continue;
						}
					}
				}
				i++;
			}
			// printf("CALL!\n");
			if (tokens[n - 1][0] == '-')
			{
				reveal(userDetails->presentDir, flag);
				return 1;
			}
			reveal(tokens[n - 1], flag);
			return 1;
		}
		if (strcmp(tokens[0], "proclore") == 0)
		{
			if (tokens[1] == NULL)
			{
				proclore(NULL);
			}
			else
			{
				proclore(tokens[1]);
			}
			return 1;
		}
		else if (strcmp(tokens[0], "seek") == 0)
		{
			int flags = 0;
			char *target = NULL;
			char directory[500] = ""; // Initialize directory to an empty string
			int count = 0;			  // Initialize coun
			for (int i = 1; i < n; i++)
			{
				if (tokens[i][0] == '-')
				{
					if (strstr(tokens[i], "d"))
						flags |= 1;
					if (strstr(tokens[i], "f"))
						flags |= 2;
					if (strstr(tokens[i], "e"))
						flags |= 4;
				}
				else if (target == NULL)
				{
					target = tokens[i];
				}
				else
				{
					strcpy(directory, tokens[i]);
				}
			}

			if (strcmp("~", directory) == 0)
			{
				strcpy(directory, userDetails->homeDir);
			}
			else if (strcmp("-", directory) == 0)
			{
				if (userDetails->lastDir[0] == '\0')
				{
					perror(RED "No last Directory" RESET);
					return 0;
				}
				strcpy(directory, userDetails->lastDir);
			}
			else if (directory[0] == '\0')
			{
				strcpy(directory, userDetails->presentDir);
			}

			if ((flags & 1) && (flags & 2))
			{
				printf("Invalid flags!\n");
				return 0;
			}

			if (!((flags & 1) || (flags & 2)))
			{
				flags |= 1;
				flags |= 2;
			}

			char *found_path = (char *)malloc(MAX_SIZE_1 * sizeof(char)); // Correct the malloc size

			if (found_path == NULL)
			{
				perror(RED "Memory allocation failed" RESET);
				return 1;
			}
			// printf("target = %s \n", target);
			// printf("flag = %d \n", flags);
			// printf("count = %d \n", count);
			// printf("found_path = %s\n", found_path);
			seek(target, directory, flags, &count, found_path);
			int found_files = count;
			if (found_files == 0)
			{
				printf("No match found!\n");
				// printf("here only\n");
				return 1;
			}
			if (flags & 4)
			{
				struct stat filestat;
				if (stat(found_path, &filestat) < 0)
				{
					perror(RED "Error in seek file stat" RESET);
					free(found_path);
					return 1;
				}

				if (found_files == 1 && ISFILE(filestat.st_mode))
				{
					FILE *file = fopen(found_path, "r");
					if (!file)
					{
						perror(RED "Missing permissions for task" RESET);
						free(found_path);
						return 1;
					}
					char ch;
					while ((ch = fgetc(file)) != EOF)
					{
						putchar(ch);
					}
					fclose(file);
				}
				else if (found_files == 1 && ISDIR(filestat.st_mode))
				{
					hop(found_path);
				}
			}
			free(found_path);
			return 1;
		}
		else if (strcmp(tokens[0], "fg") == 0)
		{
			// printf("CODE WAS HERE\n");
			fg(tokens[1]);
			return 1;
		}
		else if (strcmp(tokens[0], "bg") == 0)
		{
			if (n == 1)
			{
				perror(RED "No argument provided" RESET);
				return 0;
			}
			bg_exec(tokens[1]);
		}
		else if (strcmp(tokens[0], "activities") == 0)
		{
			printBackgroundProcesses();
		}
		else if (strcmp(tokens[0], "ping") == 0)
		{
			if (n == 3)
			{
				execute_ping(tokens[1], tokens[2]);
			}
			else
			{
				perror(RED "Invalid command\n" RESET);
			}
			return 1;
		}
		else if (strcmp(tokens[0], "neonate") == 0)
		{
			// printf("%d\n", n);
			if (n == 3)
			{
				neonate(atoi(tokens[2]));
			}
			else
			{
				perror(RED "Invalid Use of Command Usage: %s -n [time_arg]" RESET);
			}
			return 1;
		}
		else if (strcmp(tokens[0], "iMan") == 0)
		{
			if (n == 1)
			{
				perror(RED "No command entered" RESET);
			}

			if (n == 2)
			{
				fetch_man_page(tokens[1]);
			}
			return 1;
		}
		else if (check_alias(tokens[0]))
		{
			char *toreplace = check_alias(tokens[0]); // get the alias value

			// Replace the first word of the command with the alias
			tokens[0] = toreplace;

			// Construct the new command by joining tokens into a string
			char newcmd[MAX_SIZE_1] = ""; // Adjust MAX_CMD_LEN as per your requirement
			for (int i = 0; tokens[i] != NULL; i++)
			{
				strcat(newcmd, tokens[i]);
				strcat(newcmd, " "); // Add space between tokens
			}

			// Execute the new command
			execCmd(newcmd, bg);
			return 1;
			// char *toreplace = check_alias(tokens[0]);
			// replace first word of cmd with toreplace
			// 	execCmd(newcmd, bg);
		}
		// else if (check_function(cpcmd) != '\0')
		// {
		// 	// printf("here %s\n", cpcmd);
		// 	printf("%s\n", check_function(cmd));
		// 	// printf("ok bro\n");
		// 	return 1;
		// }
		else
		{
			pid_t pid = fork();
			if (pid < 0)
			{
				perror(RED "Fork failed" RESET);
				return 0;
			}
			else
			{
				// char *args[] = {"/bin/sh", "-c", cmd, NULL};
				// printf("%s, %s\n", tokens[0], tokens[1]);
				time_t start_time = time(NULL);
				setpgid(0, 0);
				if (bg)
				{ // background
					if (pid == 0)
					{ // child

						// setpgid(0, 0);
						printf("[%d]\n", getpid());
						printf("%s\n", tokens[0]);

						if (execvp(tokens[0], tokens) == -1)
						// if (execvp(args[0], args) == -1)
						{
							perror(RED "error in execvp BG" RESET);
							return 0;
						}
					}
					else
					{ // parent
						if (userDetails->ind >= 4096)
						{
							perror(RED "Too many background processes" RESET);
						}
						userDetails->myBgs[userDetails->ind].pid = pid;
						strcpy(userDetails->myBgs[userDetails->ind++].name, tokens[0]);
						return 1;
						//  dont wait
					}
					return 1;
				}
				else
				{ // 4ground
					if (pid < 0)
					{
						perror(RED "Fork failed" RESET);
						return 0;
					}
					else if (pid == 0)
					{ // child
						// printf("I am here\n");
						// setpgid(0, 0);

						if (execvp(tokens[0], tokens) == -1)
						// if (execvp(args[0], args) == -1)
						{
							perror(RED "error in execvp" RESET);
							return 0;
						}
					}
					else
					{ // parent wait here
						int status;
						// userDetails->shellPid = getppid();
						int fore_pgid = getpgrp();
						userDetails->forRunn = pid;
						if (signal(SIGTTOU, SIG_IGN) == SIG_ERR)
						{
							perror(RED "Error: Failed to ignore SIGTTOU signal." RESET);
							return 0;
						}
						// struct sigaction sa2;

						// sa2.sa_handler = SIG_IGN;  // Set to ignore the signal
						// sigemptyset(&sa2.sa_mask); // Initialize the signal set
						// sa2.sa_flags = 0;		   // No special flags

						// if (sigaction(SIGTTOU, &sa2, NULL) == -1)
						// {
						// 	perror("Error: Failed to ignore SIGTTOU signal");
						// 	return 1;
						// }

						pid_t ok = getpgid(pid);
						if (tcsetpgrp(STDIN_FILENO, ok) == -1)
						{
							perror("Error setting process group to foreground");
							return 0;
						}
						kill(-pid, SIGCONT); // all processes to continue

						// int status;
						if (waitpid(pid, &status, WUNTRACED) == -1)
						{
							// perror(RED "Error waiting for process" RESET);
							return 0;
						}
						tcsetpgrp(STDIN_FILENO, fore_pgid);

						time_t end_time = time(NULL);
						double e_time = difftime(end_time, start_time);
						// printf("%f", e_time);
						int ti = (int)e_time;

						if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
						{
							printf("%sUnknown command: %s%s\n", RED, tokens[0], RESET);
						}
						else if (e_time >= 2)
						{
							// printf("<%s > process ended normally", tokens[0]);
							char hi[100];
							snprintf(hi, 100, "<%s : %ds>", tokens[0], ti);
							strcpy(userDetails->sig, hi);
							// printf("hi here %s",hi);e
						}
					}
				}
			}
			return 1;
		}
	}
	return 1;
}

void tokenize_execute(char *ucmd)
{
	int i = 0; // reading
	int j = 0; // writing
	char *cmd = malloc(MAX_SIZE_1);
	while (ucmd[i])
	{
		cmd[j++] = ucmd[i];
		if (ucmd[i] == '&')
		{
			cmd[j++] = ';';
		}
		i++;
	}
	cmd[j] = '\0';

	char *delimiter = ";";
	char *saveptr;

	char *token = strtok_r(cmd, delimiter, &saveptr);

	char mytokens[MAX_TOKENS][MAX_SIZE_1];
	int n = 0;

	while (token != NULL)
	{
		strcpy(mytokens[n++], token);
		token = strtok_r(NULL, delimiter, &saveptr);
	}

	int stdin_backup = dup(STDIN_FILENO);
	int stdout_backup = dup(STDOUT_FILENO);
	for (int i = 0; i < n; i++)
	{
		int hasAnd = 0;
		// if (mytokens[i][strlen(mytokens[i]) - 1] == '&')
		// {
		//     hasAnd = 1;
		//     mytokens[i][strlen(mytokens[i]) - 1] = '\0';
		// }
		check_pipe_execute(mytokens[i], hasAnd);
	}

	restoreIO(stdin_backup, stdout_backup);
	return;
}
