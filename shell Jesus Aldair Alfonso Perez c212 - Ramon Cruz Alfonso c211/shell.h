#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>

#define TRUE 1
#define FALSE !TRUE

#ifndef MAX
#define MAX 1024
#endif

static char buffer[MAX];

// Shell pid, pgid, terminal modes
static pid_t GBSH_PID;
static pid_t GBSH_PGID;
static int GBSH_IS_INTERACTIVE;
static struct termios GBSH_TMODES;
const char * historyFileName = "history.txt";
static int historyCount;
static char * actualHistory;


static char* currentDirectory;
extern char** environ;

struct sigaction act_child;
struct sigaction act_int;

int no_reprint_prmpt;

pid_t pid;


/**
 * SIGNAL HANDLERS
 */
// signal handler for SIGCHLD */
void signalHandler_child(int p);
// signal handler for SIGINT
void signalHandler_int(int p);


int changeDirectory(char * args[]);

int parseline(char* buf, char** argv);
void eval(char* cmdline);
int builtin_command(char** argv);
void saveHistory(char* args);
void loadHistory();
