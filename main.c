#include "headers.h"
#include "sh_prompt.h"
#include "sh_pwd.h"
#include "sh_echo.h"
#include "sh_error.h"
#include "sh_cd.h"
#include "sh_history.h"
#include "sh_ls.h"
#include "sh_fg.h"
#include "sh_bg.h"
#include "sh_pinfo.h"
#include "sh_discover.h"
#include "sh_redir.h"
#include "sh_jobs.h"
#include "sh_justBG.h"
#include "sh_justFG.h"
#include "sh_signal.h"
#include "sh_autoComplete.h"

struct timespec start, finish;
struct utsname loginData;
char userName[MAX_LENGTH] = {};
char systemName[MAX_LENGTH] = {};
char pwd[MAX_LENGTH] = {};
char absPWD[MAX_LENGTH] = {};
char home[MAX_LENGTH] = {};
char prev_dir[MAX_LENGTH] = {};
char history[HISTORY_LENGTH][MAX_LENGTH] = {};
char *bgProc[MAX_BG_PROCESS];
int bgPID[MAX_BG_PROCESS] = {};
int historyStart = 0;
int numBGProc = 0;
int fgRunning = 0;
int fgPID = 0;
char fgProc[MAX_LENGTH]={};
double time_taken;
int OGstdin;
int OGstdout;
int newIn = 0;
int newOut = 0;
int sigCall = 0;
int mainPID;
int sigZ = 0;
int findBgNum();

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

void sigtstp_handler(int signum)
{
    if (!fgRunning)
    {
        return;
    }

    kill(fgRunning, SIGSTOP);
    int bgNum = findBgNum();
    bgPID[bgNum] = fgRunning;
    bgProc[bgNum] = fgProc;
    sigZ = 1;
    return;
}

void sigint_handler(int signum)
{
    if (!fgRunning)
        return;

    int status;
    sigCall = 1;
    kill(fgRunning, SIGINT);
    fgRunning = 0;
    return;
}

void sigquit_handler(int sugnum)
{
    while (1)
        _exit(0);
    while(1)
        exit(0);
    return;
}

int getRawInput(char* inp)
{
    char c;
    setbuf(stdout, NULL);
    enableRawMode();
    memset(inp, '\0', 100);
    int pt = 0;
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (iscntrl(c))
        {
            if (c == 10)
                break;
            else if (c == 27)
            {
                char buf[3];
                buf[2] = 0;
                if (read(STDIN_FILENO, buf, 2) == 2)
                {
                    printf("\rarrow key: %s", buf);
                }
            }
            else if (c == 127)
            { // backspace
                if (pt > 0)
                {
                    if (inp[pt - 1] == 9)
                    {
                        for (int i = 0; i < 7; i++)
                        {
                            printf("\b");
                        }
                    }
                    inp[--pt] = '\0';
                    printf("\b \b");
                }
            }
            else if (c == 9)
            { // TAB character
                int prevPt = pt;
                int retval = sh_autoComplete(inp, &pt);

                if (retval == 1)
                    fprintf(stdout, "\r\033[0;32m<%s@%s\033[0m\033[0;34m:%s\033[0;32m>\033[0m %s", userName, systemName, pwd, inp);

                else
                {
                    for(int i=prevPt; i<pt; i++)
                        printf("%c", inp[i]);
                }
            }
            else if (c == 4)
            {
                // kill(mainPID, SIGKILL);
                while(1) _exit(0);
                while(1) exit(0);
            }
            else
            {
                printf("%d\n", c);
            }
        }
        else
        {
            inp[pt++] = c;
            printf("%c", c);
        }
    }
    disableRawMode();
    printf("\n");
    return 0;
}

int cls()
{
    printf("\e[1;1H\e[2J");
    return 0;
}

int getln(char *inputLine)
{
    int tempCnt = 0;
    char c;
    scanf("%c", &c);
    while (c != '\n' && c != '\r')
    {
        inputLine[tempCnt++] = c;
        scanf("%c", &c);
        if (c == '\n')
            break;
    }
    inputLine[tempCnt] = '\0';
    return 0;
}

int println(char *printLine)
{
    int cnt = 0;
    char c = printLine[cnt++];
    while (c != '\n' && c != '\0')
    {
        printf("%c", c);
        c = printLine[cnt++];
    }
    printf("\n");
    return 0;
}

void bgProcHandler(int signum)
{
    if (sigZ)
    {
        sigZ = 0;
        debug("here");
        fgRunning = 0;
        return;
    }
    if (sigCall)
    {
        fgRunning = 0;
        sigCall = 0;
        return;
    }
    if (fgRunning == -1)
    {
        fgRunning = 0;
        return;
    }

    int status;
    int pid = waitpid(-1, &status, WNOHANG);

    int processNum = 0;
    while (processNum < MAX_BG_PROCESS && bgPID[processNum] != pid)
        processNum++;

    if (processNum == MAX_BG_PROCESS)
    {
        fgRunning = 0;
        return;
    }

    if(bgPID[processNum] == 0)
    {
        fgRunning = 0;
        bgPID[processNum] = 0;
        bgProc[processNum] = NULL;
        return;
    }

    char timePrompt[MAX_LENGTH] = {};
    fprintf(stderr, "\r[%d]  + %d %s       %-150s\n", processNum + 1, bgPID[processNum], WIFEXITED(status) ? "done" : "aborted", bgProc[processNum]);
    if (!fgRunning)
        fprintf(stderr, "\r\033[0;32m<%s@%s\033[0m\033[0;34m:%s\033[0;32m>\033[0m ", userName, systemName, pwd);

    bgPID[processNum] = 0;
    bgProc[processNum] = NULL;

    return;
}

int updatePWD()
{
    char temp[MAX_LENGTH - 4];
    char temp2[MAX_LENGTH - 2];
    getcwd(temp, MAX_LENGTH - 1);
    if (!strncmp(temp, home, strlen(home)))
    {
        strcpy(temp2, temp + strlen(home));
        sprintf(pwd, "~%s", temp2);
    }
    else
    {
        strcpy(pwd, temp);
    }
    strcpy(absPWD, temp);
    return 0;
}

int chkValid(char *inpCmd)
{
    char inpCmdCopy[MAX_LENGTH];
    char modInpCmd[MAX_LENGTH];
    strcpy(inpCmdCopy, inpCmd);

    int retval = sh_redir(inpCmdCopy);
    if (retval == ERR_FILE_DNE)
    {
        return retval;
    }
    strcpy(modInpCmd, inpCmdCopy);
    inpCmd = modInpCmd;

    char delimit[] = " \n\t\r\v\f";
    char *token = NULL;
    while (!token)
        token = strtok(inpCmd, delimit);

    if (token == NULL)
    {
        sh_standardizeInOut();
        return 0;
    }

    if (!strcmp(token, "echo"))
    {
        token = strtok(NULL, ""); // splitting on a non-existing delimiter to get remaining string fully
        if (token == NULL)
            sh_echo(" ");
        else
            sh_echo(token);
        sh_standardizeInOut();
    }

    else if (!strcmp(token, "pwd"))
    {
        token = strtok(NULL, ""); // splitting on a non-existing delimiter to get remaining string fully
        if (token != NULL)
        {
            sh_standardizeInOut();
            sh_error(token, ERR_EXTRA_PARAMETER);
            return ERR_EXTRA_PARAMETER;
        }
        sh_pwd(absPWD);
        sh_standardizeInOut();
    }

    else if (!strcmp(token, "cd"))
    {
        token = strtok(NULL, ""); // splitting on a non-existing delimiter to get remaining string fully
        if (token == NULL)
        {
            sh_standardizeInOut();
            return 0;
        }

        int retval = sh_cd(token);
        sh_standardizeInOut();
        if (retval == ERR_EXTRA_PARAMETER)
            sh_error("cd", ERR_EXTRA_PARAMETER);
        else if (retval)
            sh_error(token, retval); // path is the invalid directory

        updatePWD();
    }

    else if (!strcmp(token, "history"))
    {
        token = strtok(NULL, ""); // splitting on a non-existing delimiter to get remaining string fully
        int retval = sh_history(token);
        sh_standardizeInOut();
        if (retval)
            sh_error("history", ERR_INVALID_PARAMETER);

        return 0;
    }

    else if (!strcmp(token, "clear"))
    {
        token = strtok(NULL, "");
        if (token != NULL)
        {
            sh_standardizeInOut();
            sh_error(token, ERR_EXTRA_PARAMETER);
            return ERR_EXTRA_PARAMETER;
        }
        cls();
    }

    else if (!strcmp(token, "ls"))
    {
        token = strtok(NULL, "");
        int retval = sh_ls(token, absPWD);
        sh_standardizeInOut();
        chdir(pwd);
        if (retval)
            sh_error("ls", retval);
    }

    else if (!strcmp(token, "pinfo"))
    {
        int retval = 0;
        token = strtok(NULL, "");
        char *pid;
        if (token == NULL)
            pid = "self";
        else
            pid = token;

        retval = sh_pinfo(pid);
        sh_standardizeInOut();
        if (retval)
        {
            sh_error("pinfo", retval);
            return retval;
        }
    }

    else if (!strcmp(token, "discover"))
    {
        int retval = 0;
        token = strtok(NULL, "");
        retval = sh_discover(token);
        sh_standardizeInOut();
        if (retval)
        {
            sh_error("discover", retval);
            return retval;
        }
    }

    else if (!strcmp(token, "jobs"))
    {
        int retval = 0;
        token = strtok(NULL, "");
        sh_jobs(token);
        if (retval)
        {
            sh_error("jobs", retval);
            return retval;
        }
    }

    else if (!strcmp(token, "bg"))
    {
        int retval = 0;
        token = strtok(NULL, delimit);
        if (token == NULL)
        {
            sh_error("bg", ERR_MISSING_PARAMETER);
            return ERR_MISSING_PARAMETER;
        }

        int jobNum = atoi(token);
        token = strtok(NULL, delimit);
        if (token != NULL)
        {
            sh_error("bg", ERR_EXTRA_PARAMETER);
            return ERR_EXTRA_PARAMETER;
        }

        if (jobNum <= 0 || jobNum >= MAX_BG_PROCESS)
        {
            return ERR_INVALID_JOBNUM;
        }

        retval = sh_justBG(bgPID[jobNum - 1]);
        if (retval)
        {
            debug("Implement invalid jobNum error");
            sh_error("bg", ERR_INVALID_JOBNUM);
            return ERR_INVALID_JOBNUM;
        }
    }

    else if (!strcmp(token, "fg"))
    {
        int retval = 0;
        token = strtok(NULL, delimit);
        // token = strtok(NULL, "");

        if (token == NULL)
        {
            sh_error("fg", ERR_MISSING_PARAMETER);
            return ERR_MISSING_PARAMETER;
        }

        int jobNum = atoi(token);
        token = strtok(NULL, delimit);
        if (token != NULL)
        {
            sh_error("fg", ERR_EXTRA_PARAMETER);
            return ERR_EXTRA_PARAMETER;
        }

        if (jobNum <= 0 || jobNum >= MAX_BG_PROCESS)
        {
            return ERR_INVALID_JOBNUM;
        }
        int pid = bgPID[jobNum - 1];

        retval = sh_justFG(pid);

        if (retval == ERR_INVALID_JOBNUM)
        {
            sh_error("fg", ERR_INVALID_JOBNUM);
            return ERR_INVALID_JOBNUM;
        }
        bgPID[jobNum - 1] = 0;  
        bgProc[jobNum - 1] = NULL;
        return 0;
    }

    else if (!strcmp(token, "sig"))
    {
        token = strtok(NULL, delimit);
        int jobNum, sig_Num;
        jobNum = atoi(token);
        if (jobNum == 0)
        {
            sh_error("sig", ERR_MISSING_PARAMETER);
            return ERR_MISSING_PARAMETER;
        }
        token = strtok(NULL, delimit);
        sig_Num = atoi(token);
        if (sig_Num == 0)
        {
            sh_error("sig", ERR_MISSING_PARAMETER);
            return ERR_MISSING_PARAMETER;
        }
        token = strtok(NULL, delimit);
        if (token != NULL)
        {
            sh_error("sig", ERR_EXTRA_PARAMETER);
            return ERR_EXTRA_PARAMETER;
        }

        sigCall = 1;
        sh_signal(bgPID[jobNum - 1], sig_Num);
    }

    else
    {
        clock_gettime(CLOCK_REALTIME, &start);
        strcpy(fgProc, inpCmd);
        int retval = sh_fg(inpCmdCopy);
        fgRunning = 0;
        if (retval)
        {
            sh_error(inpCmd, ERR_PROCESS);
            return ERR_PROCESS;
        }

        clock_gettime(CLOCK_REALTIME, &finish);
        time_taken = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000 - 0.05;
        time_taken = (int) time_taken+1;
        retval = (int)(time_taken + 1);
        sh_standardizeInOut();
        if (retval > 1)
            return retval;
    }
    sh_standardizeInOut();
    newIn = 0;
    newOut = 0;
    return 0;
}

int findBgNum()
{
    for (int i = 0; i < MAX_BG_PROCESS; i++)
    {
        if (!bgPID[i])
            return i;
    }
    return -1;
}

int findPipedCmds(char inpCmd[MAX_LENGTH], int bgFlag)
{
    int retval=0;
    char** args = calloc(MAX_INPUT_CMDS, sizeof(char*));
    for(int i=0; i<MAX_INPUT_CMDS; i++)
        args[i] = calloc(MAX_FILENAME, sizeof(char));


    int numPipes=0, pipeNum=0, numCmds=0;
    char inpCpy[MAX_LENGTH];
    strcpy(inpCpy, inpCmd);
    char* token;
    char* delimit = "|";

    for(int i=0; i<strlen(inpCmd); i++)
    {
        if (inpCmd[i] == '|')
            numPipes++;
    }

    token = strtok(inpCpy, delimit);
    while (token != NULL)
    {
        strcpy(args[numCmds], token);
        numCmds++;
        token = strtok(NULL, delimit);
    }

    int filed[numCmds][2];
    for(int i=0; i<numCmds; i++)
    {
        OGstdin = dup(STDIN_FILENO);
        OGstdout = dup(STDOUT_FILENO);

        if (i!=(numCmds-1))
        {     
            pipe(filed[i]);
            dup2(filed[i][1], STDOUT_FILENO);
            close(filed[i][1]);
        }

        if (i!=0)
        {
            dup2(filed[i-1][0], STDIN_FILENO);
            close(filed[i-1][0]);
        }

        if (!bgFlag)
        {
            retval = chkValid(args[i]);
        }
        else
        {
            sh_redir(args[i]);
            int pid, bgNum = findBgNum();
            sh_bg(args[i], &pid, bgNum);
        }


        dup2(OGstdin, STDIN_FILENO);
        dup2(OGstdout, STDOUT_FILENO);
        close(OGstdin);
        close(OGstdout);
    }
    return retval;
}

int chkBG(char *inpCmd)
{
    int retval = 0;
    int numAnd = 0;
    for (int i = 0; inpCmd[i] != '\0'; i++)
    {
        if (inpCmd[i] == '&')
            numAnd++;
    }

    char **args = calloc(MAX_INPUT_CMDS, sizeof(char *));
    for (int i = 0; i < MAX_INPUT_CMDS; i++)
        args[i] = calloc(MAX_FILENAME, sizeof(char));

    char *token;
    char *delimit = "&";
    token = strtok(inpCmd, delimit);

    int numArgs = 0;
    while (token != NULL)
    {
        strcpy(args[numArgs++], token);
        token = strtok(NULL, delimit);
    }

    for (int i = 0; i < numAnd; i++)
    {
        int pid;
        int bgNum = findBgNum();

        retval = findPipedCmds(args[i], 1);
        if (retval == ERR_FILE_DNE)
        {
            sh_error(args[i], ERR_FILE_DNE);
            return 0;
        }

        sh_standardizeInOut();
    }

    if (args[numAnd][0] != '\0')
    {
        retval = findPipedCmds(args[numAnd], 0);
    }
    for (int i = 0; i < MAX_INPUT_CMDS; i++)
        free(args[i]);

    free(args);
    return retval;
}

int readInput()
{
    int retval = 0;
    char inputLine[MAX_LENGTH];
    // getln(inputLine);
    getRawInput(inputLine);
    char inputCopy[MAX_LENGTH], inputCopy2[MAX_LENGTH];

    strcpy(inputCopy, inputLine);
    strcpy(inputCopy2, inputLine);
    char *tempToken = strtok(inputCopy, " \n\t\r\v\f");

    if (tempToken != NULL)
        makeHistory(inputLine);

    char delimit[] = ";";
    char *pch;
    pch = strtok(inputLine, delimit);

    char *tempCmds[MAX_INPUT_CMDS] = {};
    int numCmds = 0;

    while (pch != NULL)
    {
        tempCmds[numCmds++] = pch;
        pch = strtok(NULL, delimit);
    }

    for (int i = 0; i < numCmds; i++)
        retval = chkBG(tempCmds[i]);

    return retval;
}

int getLoginData()
{
    strcpy(userName, getlogin());
    if (uname(&loginData) < 0)
    {
        sh_error(" ", ERR_INVALID_LOGIN);
        return ERR_INVALID_LOGIN;
    }
    strcpy(systemName, loginData.nodename);
    return 0;
}

int shell()
{
    int retval = 0;
    signal(SIGCHLD, bgProcHandler);
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    while (1)
    {
        newIn = 0;
        newOut = 0;
        chdir(home);
        getHist();
        chdir(pwd);
        promptData(userName, systemName, pwd, (int)time_taken);
        time_taken = 0;
        retval = readInput();
        chdir(home);
        storeHist();
        chdir(pwd);
        sh_standardizeInOut();
    }
    return 0;
}

int main()
{
    mainPID = getpid();
    signal(SIGQUIT, sigquit_handler);
    getcwd(home, MAX_LENGTH - 1);
    getcwd(prev_dir, MAX_LENGTH - 1);
    updatePWD();
    getLoginData();
    cls();
    shell();
}
