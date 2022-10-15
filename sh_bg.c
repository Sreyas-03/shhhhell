#include "headers.h"
#include "sh_bg.h"
#include "sh_prompt.h"

int sh_bg(char* inpCmd, int* r_pid, int cmdNum)
{
    char inpCmdCopy[MAX_LENGTH];
    char* args[MAX_INPUT_CMDS];

    strcpy(inpCmdCopy, inpCmd);
    int numArgs = 0;
    char* delimit = " \n\t\r\v\f";
    char* token = strtok(inpCmdCopy, delimit);
    args[numArgs++]=token;
    while (token != NULL)
    {
        token = strtok(NULL, delimit);
        args[numArgs++] = token;
    }

    if(strlen(inpCmd) > 1)
    {
        int pid = fork();
        if(pid == -1)
        {
            return ERR_BG_PROCESS;
        }
        if (pid == 0)
        {
            setpgid(getpid(), getpid());
            int retval = execvp(args[0], args);
            while (retval < 0)
                _exit(0);
            return ERR_BG_PROCESS;
        }
        bgProc[cmdNum] = (char*)malloc(strlen(inpCmd));
        strcpy(bgProc[cmdNum], inpCmd);
        bgPID[cmdNum] = pid;
        printf(" [%d] %d\n", cmdNum+1, pid);
        return 0;
    }
    return 0;
}