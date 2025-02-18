
#ifndef INPUTANDPROMPT
#define INPUTANDPROMPT

#define MAX_SIZE_1 4096
#include <stdbool.h>
// #define logFile "log.txt"
#define logFile "/log.txt"

typedef struct bgs
{
	char name[1000];
	int pid;
} bgs;

typedef struct
{
	char userName[MAX_SIZE_1];
	char systemName[MAX_SIZE_1];

	char invokedDir[MAX_SIZE_1];

	char homeDir[MAX_SIZE_1];
	char lastDir[MAX_SIZE_1];
	char presentDir[MAX_SIZE_1];
	char sig[100];

	bgs myBgs[4096];
	int ind;

	pid_t forRunn;
	// pid_t shellPid;

} User_Details;


extern User_Details *userDetails;

void getUserDetails();
void printPrompt();
void getCommand(char *cmd);

// Text color
#define RESET "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"

// Bold text color
#define BOLD_BLACK "\033[1;30m"
#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_BLUE "\033[1;34m"
#define BOLD_MAGENTA "\033[1;35m"
#define BOLD_CYAN "\033[1;36m"
#define BOLD_WHITE "\033[1;37m"

#define FG_GREEN "\033[32m"
#define FG_BLUE "\033[34m"
#define FG_BLACK "\033[30m"
#define BG_GREEN "\033[42m"
#define BG_BLUE "\033[44m"
#define FG_DEFAULT "\033[39m"
#define BG_DEFAULT "\033[49m"

bool hop(char *path);
bool reveal(char *path, int flag);
bool log_write(char *cmd);
bool log_print();
bool log_purge();
bool log_exec(int n, char *log);

void proclore(char *pid);
void seek(const char *target, const char *directory, int flags, int *count, char *found_path);
void tokenize_execute(char *ucmd);
int execCmd(char *cmd, int bg);

void redirectIO(char *file, int flag);
void restoreIO(int stdin_backup, int stdout_backup);

void fg(char *pid_str);
void printBackgroundProcesses(); 
void activities();

void check_pipe_execute(char *execmd, int hasAnd);
void execute_ping(char* pid_str, char* sig_str);

void fetch_man_page(const char *command);
void bg_exec(char *pid_str);
// void remove_bg_process(int pid);
void neonate(int n);
void alias();
char* check_alias(char *cmd);

#endif
