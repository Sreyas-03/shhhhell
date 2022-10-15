#include "headers.h"
#include "sh_fg.h"
#include "sh_error.h"

int sh_fg(char* inpCmd)
{
    char** args = calloc(MAX_INPUT_CMDS, sizeof(char*));
    for(int i=0; i<MAX_INPUT_CMDS; i++)
        args[i] = calloc(MAX_FILENAME, sizeof(char));

    int numArgs=0;
    char* delimit =  " \n\t\r\v\f";
    char* token = strtok(inpCmd, delimit);
    while (token != NULL)
    {
        strcpy(args[numArgs], token);
        numArgs++;
        token = strtok(NULL, delimit);
    }
    args[numArgs]=NULL;

    int time_spent = 0;
    clock_t begin = clock();
    int pid=fork();
    if (pid==-1)
    {
        return ERR_PROCESS;
    }
    if (pid==0)
    {
        int retval = execvp(args[0], args);
        if (retval == -1)
        {
            sh_error(args[0], ERR_INVALID_CMD);
            while (1) _exit(0);
        }
    }

    else
    {
        int status, run_time;
        fgRunning = pid;
        waitpid(pid, &status, WUNTRACED);
        // kill(pid, SIGKILL);
    }
    return 0;
}
