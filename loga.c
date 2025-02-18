#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/utsname.h>

#include "__my_headers__.h"

bool log_exec(int n, char *log)
{
    char logpath[MAX_SIZE_1 + 10];
    snprintf(logpath, sizeof(logpath), "%s/log.txt", userDetails->homeDir);

    if (n > 15 || n < 0)
    {
        printf(RED "INVALID N : n should be less than 15\n" RESET);
        return false;
    }

    FILE *fp = fopen(logpath, "r");
    if (fp == NULL)
    {
        perror(RED "cant open file" RESET);
        return false;
    }

    char **logs = malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++)
    {
        logs[i] = malloc(MAX_SIZE_1);
        logs[i][0] = '\0';
    }

    int lcnt = 0;
    char str[MAX_SIZE_1];

    while (fgets(str, MAX_SIZE_1, fp) != NULL)
    {
        if (str[0] == '\n')
        {
            continue;
        }
        str[strcspn(str, "\n")] = '\0';

        strcpy(logs[lcnt % n], str);
        lcnt++;
    }

    fclose(fp);
    if (lcnt < n)
    {
        fprintf(stderr, "not enough commands.\n");
        for (int i = 0; i < n; i++)
        {
            free(logs[i]);
        }
        free(logs);
        return false;
    }
    strcpy(log, logs[(lcnt - n) % n]);
    for (int i = 0; i < n; i++)
    {
        free(logs[i]);
    }
    free(logs);

    return true;
}

bool log_purge()
{
    char *logpath = malloc(sizeof(char) * (MAX_SIZE_1 + 10));
    sprintf(logpath, "%s/log.txt", userDetails->homeDir);
    FILE *fp = fopen(logpath, "w");
    // make file empty
    // while (fgetc(fp) != EOF)
    // {
    //     fprintf(fp, "\n");
    // }
    // fseek(fp, 0, SEEK_SET);
    int cnt = 0;
    // while (cnt < 15)
    // {
    //     fprintf(fp, "\n");
    //     cnt++;
    // }
    fclose(fp);
    return true;
}
bool log_print()
{
    char *logpath = malloc(sizeof(char) * (MAX_SIZE_1 + 10));
    sprintf(logpath, "%s/log.txt", userDetails->homeDir);
    FILE *fp = fopen(logpath, "r");
    if (fp == NULL)
    {
        return false;
    }
    char str[MAX_SIZE_1];
    fseek(fp, 0, SEEK_SET);
    while (fgets(str, MAX_SIZE_1, fp) != NULL)
    {
        if (str[0] == '\n')
        {
            continue;
        }
        printf("%s", str);
    }
    fclose(fp);
    return true;
}

bool log_write(char *cmd)
{
    if (strcmp(cmd, "\n") == 0 || strcmp(cmd, "") == 0)
    {
        return 1;
    }

    char *logpath = malloc(sizeof(char) * (MAX_SIZE_1 + 10));
    sprintf(logpath, "%s/log.txt", userDetails->homeDir);
    // if (strstr(cmd, "log") != NULL)
    // {
    //     return true;
    // }
    FILE *fp = fopen(logpath, "r+");
    if (!fp)
    {
        fp = fopen(logpath, "w+");
        if (!fp)
        {
            perror(RED "cant open file" RESET);
            return false;
        }
    }

    int cnt = 0;
    char ch;
    fseek(fp, 0, SEEK_SET);
    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n')
        {
            cnt++;
        }
    }
    while (cnt < 15)
    {
        fprintf(fp, "\n");
        cnt++;
    }
    // char **log = (char **)malloc(sizeof(char *) * 15);
    // for (int i = 0; i < 15; i++)
    // {
    //     log[i] = (char *)malloc(sizeof(char) * MAX_SIZE_1);
    // }
    char log[15][MAX_SIZE_1];

    fseek(fp, 0, SEEK_SET);

    for (int i = 0; i < 15; i++)
    {
        if (fgets(log[i], MAX_SIZE_1, fp) == NULL)
        {
            log[i][0] = '\0';
        }
        log[i][strcspn(log[i], "\n")] = 0;
    }
    if (strcmp(log[14], cmd) == 0)
    {
        // for (int i = 0; i < 15; i++)
        // {
        //     free(log[i]);
        // }
        // free(log);
        fclose(fp);
        return true;
    }

    for (int i = 0; i < 14; i++)
    {
        strcpy(log[i], log[i + 1]);
    }
    strcpy(log[14], cmd);

    fseek(fp, 0, SEEK_SET);

    // fclose(fp);
    // fp = fopen(logpath, "w");
    // if (!fp)
    // {
    //     perror(RED "cant open file" RESET);
    //     return false;
    // }

    fclose(fp);      // Correct way to close a file pointer
    remove(logpath); // Correct way to remove a file using its path

    fp = fopen(logpath, "r+");
    if (!fp)
    {
        fp = fopen(logpath, "w+");
        if (!fp)
        {
            perror("can't open file");
            return false;
        }
    }
    // remove(fp);
    // close(fp);
    // fp = fopen(logpath, "r+");
    // if (!fp)
    // {
    //     fp = fopen(logpath, "w+");
    //     if (!fp)
    //     {
    //         perror(RED "cant open file" RESET);
    //         return false;
    //     }
    // }

    for (int i = 0; i < 15; i++)
    {
        fprintf(fp, "%s\n", log[i]);
    }

    // for (int i = 0; i < 15; i++)
    // {
    //     free(log[i]);
    // }
    // free(log);
    fclose(fp);
    return true;
}