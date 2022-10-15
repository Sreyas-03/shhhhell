#include <stdlib.h>
#include <dirent.h>
#include <sys/utsname.h>
#include <grp.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pwd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <ctype.h>
#include <string.h>

#define MAX_LENGTH 1024
#define MAX_INPUT_CMDS 50
#define HISTORY_LENGTH 20
#define DATE_TIME_LENGTH 10
#define MAX_FILENAME 50
#define MAX_BG_PROCESS 20

#define ERR_MISSING_PARAMETER -1
#define ERR_EXTRA_PARAMETER -2
#define ERR_INVALID_CMD -3
#define ERR_INVALID_DIRECTORY_CD -4
#define ERR_INVALID_PARAMETER -5
#define ERR_INVALID_PATH -6
#define ERR_INVALID_FLAG -7
#define ERR_BG_PROCESS -8
#define ERR_PROCESS -9
#define ERR_FILE_OPEN -10
#define ERR_INVALID_PID -11
#define ERR_INVALID_LOGIN -12
#define ERR_FILE_NOT_FOUND -13
#define ERR_FILE_DNE -14
#define ERR_INVALID_JOBNUM -15
#define ERR_INVALIDPIPE -16

#define debug(i) {printf("%s\n", i);fflush(stdout);}
#define deb(i) {printf("%s = %d\n", #i, i); fflush(stdout);}

extern struct timespec start, finish;
extern char home[MAX_LENGTH];
extern char prev_dir[MAX_LENGTH];
extern char history[HISTORY_LENGTH][MAX_LENGTH];
extern int historyStart;
extern int bgPID[MAX_BG_PROCESS];
extern char* bgProc[MAX_BG_PROCESS];
extern int OGstdin;
extern int OGstdout;
extern int newIn;
extern int newOut;
extern char* bgProc[MAX_BG_PROCESS];
extern int bgPID[MAX_BG_PROCESS];
extern int fgRunning;
extern double time_taken;