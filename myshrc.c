#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 256
#define MAX_ALIAS 100
#define MAX_FUNCTIONS 10

#define MAX_ARGS 10
#define MAX_BODY_LEN 1024

#include "__my_headers__.h"
// Struct to hold aliases
typedef struct
{
    char alias_name[MAX_LINE_LEN];
    char command[MAX_LINE_LEN];
} Alias;

// Struct to hold functions
typedef struct
{
    char func_name[MAX_LINE_LEN];
    char body[MAX_LINE_LEN];
} Function;

Alias aliases[MAX_ALIAS];
Function functions[MAX_FUNCTIONS];
int alias_count = 0;
int func_count = 0;
// Function functions[] = {
//     {"mk_hop", "mkdir \"$1\"; hop \"$1\""},
//     {"hop_seek", "hop \"$1\"; seek \"$1\""}};
// int func_count = 2;

// Function to add alias
void add_alias(const char *alias_name, const char *command)
{
    if (alias_count < MAX_ALIAS)
    {
        strcpy(aliases[alias_count].alias_name, alias_name);
        strcpy(aliases[alias_count].command, command);
        alias_count++;
    }
}

// Function to add function
void add_function(const char *func_name, const char *body)
{
    if (func_count < MAX_FUNCTIONS)
    {
        strcpy(functions[func_count].func_name, func_name);
        strcpy(functions[func_count].body, body);
        func_count++;
    }
}

// Function to parse the .myshrc file
void parse_myshrc(FILE *file)
{
    char line[MAX_LINE_LEN];

    while (fgets(line, MAX_LINE_LEN, file) != NULL)
    {
        // Ignore comments
        if (line[0] == '#')
            continue;
        // if (line[0] == '//')
        if (strncmp(line, "//", 2) == 0)
            continue;

        // Trim newline character at the end
        line[strcspn(line, "\n")] = 0;

        // Check for alias
        if (strstr(line, "=") != NULL)
        {
            char *token = strtok(line, "=");
            char alias_name[MAX_LINE_LEN], command[MAX_LINE_LEN];
            strcpy(alias_name, token);
            token = strtok(NULL, "=");
            strcpy(command, token);

            add_alias(alias_name, command);
        }
        // Check for function definition (starts with "func" or function name followed by "()")
        else if (strstr(line, "()") != NULL || strstr(line, "func") != NULL)
        {
            char func_name[MAX_LINE_LEN];
            strcpy(func_name, strtok(line, "()"));
            char body[MAX_LINE_LEN] = "";

            // Read the function body (next few lines)
            while (fgets(line, MAX_LINE_LEN, file) != NULL)
            {
                if (line[0] == '}')
                    break;
                strcat(body, line); // Append function body
            }
            add_function(func_name, body);
        }
    }
}

// Example function array

// Function to replace $1, $2, ... with arguments from cmd
// char *replace_arguments(const char *body, char **args, int arg_count)
// {
//     // return "HIHI";
//     static char new_body[MAX_BODY_LEN];
//     memset(new_body, 0, MAX_BODY_LEN);
//     const char *p = body;
//     char temp[MAX_BODY_LEN];
//     printf("argscount = %d\n", arg_count);
//     for (int i = 0; i < arg_count - 1; i++)
//     {
//         printf("%d : %s \n", i, args[i]);
//     }

//     // return "HEY";

//     // while (*p)
//     // {
//     //     if (*p == '$' && *(p + 1) >= '1' && *(p + 1) <= '9')
//     //     {
//     //         int arg_index = *(p + 1) - '1'; // Extract the argument number (0-based)
//     //         if (arg_index < arg_count)
//     //         {
//     //             strcat(new_body, args[arg_index]); // Replace $n with the corresponding argument
//     //         }
//     //         p += 2; // Move past the $ and the number
//     //     }
//     //     else
//     //     {
//     //         strncat(new_body, p, 1);
//     //         p++;
//     //     }
//     // }

//     return new_body;
// }

// char *check_function(char *cmd)
// {
//     // return "hi";

//     // Tokenize the command to get the function name and arguments
//     // char *cmd_copy = strdup(cmd);
//     char cmd_copy[MAX_SIZE_1];
//     int i = 0;
//     while (cmd[i] != '\0')
//     {
//         cmd_copy[i] = cmd[i];
//         i++;
//     }

//     // strcpy(cmd_copy, cmd);
//     printf("copy cmd %s\n", cmd_copy);
//     // return "hi";
//     char *token = strtok(cmd_copy, " ");
//     char func_name[MAX_SIZE_1];
//     strcpy(func_name, token);

//     char *args[MAX_ARGS];
//     int arg_count = 0;
//     // return "hi";

//     // Collect arguments from cmd
//     while (token != NULL && arg_count < MAX_ARGS)
//     {
//         token = strtok(NULL, " ");
//         if (token != NULL)
//         {
//             args[arg_count++] = token;
//         }
//     }
//     for (int i = 0; i < arg_count; i++)
//     {
//         printf("%d %s\n", i, args[i]);
//     }

//     // Find the function and replace arguments
//     for (int i = 0; i < func_count; i++)
//     {
//         // printf("i\n");
//         // printf("%s\n",functions[i].func_name);
//         if (strcmp(functions[i].func_name, func_name) == 0)
//         {
//             // printf("func %s\n{\n", functions[i].func_name);
//             return replace_arguments(functions[i].body, args, arg_count);
//             // return "done";
//             // printf("%s\n}\n", newcmd);
//             // free(cmd_copy);
//             // return newcmd;
//         }
//     }
//     return "hereary";

//     // free(cmd_copy);
//     return '\0'; // Function not found
// }

// void execute_function(const char *func_name, const char *arg)
// {
//     for (int i = 0; i < func_count; i++)
//     {
//         if (strcmp(functions[i].func_name, func_name) == 0)
//         {
//             printf("Executing function: %s with arg %s\n", func_name, arg);
//             printf("Function body:\n%s\n", functions[i].body);
//             // Simulate function execution logic
//             if (strcmp(func_name, "mk_hop") == 0)
//             {
//                 printf("Simulating mkdir for directory: %s\n", arg);
//                 printf("Simulating hop into directory: %s\n", arg);
//             }
//             else if (strcmp(func_name, "hop_seek") == 0)
//             {
//                 printf("Simulating hop into directory: %s\n", arg);
//                 printf("Simulating seek in directory: %s\n", arg);
//             }
//             return;
//         }
//     }
//     printf("Function not found: %s\n", func_name);
// }

char *check_alias(char *cmd)
{
    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(aliases[i].alias_name, cmd) == 0)
        {
            // printf("Alias found: %s -> %s\n", cmd, aliases[i].command);
            return aliases[i].command;
            // Simulate alias execution by printing the corresponding command
            // printf("Simulating execution of command: %s\n", aliases[i].command);
        }
    }
    // printf("Alias not found: %s\n", cmd);
    return NULL;
}

// int main() {
void alias()
{
    FILE *file = fopen(".myshrc", "r");
    if (!file)
    {
        printf("Failed to open .myshrc file.\n");
        return;
    }

    // Parse the .myshrc file
    parse_myshrc(file);
    fclose(file);

    // Simulate execution of some aliases/functions
    // printf("Testing alias 'home':\n");
    // execute_alias("home");
    // execute_alias("reveal");

    // printf("\nTesting function 'mk_hop' with argument 'testdir':\n");
    // execute_function("mk_hop", "testdir");

    // printf("\nTesting function 'hop_seek' with argument 'testdir':\n");
    // execute_function("hop_seek", "testdir");

    return;
}
