#include "headers.h"
#include "sh_justBG.h"

int sh_justBG(int pid)
{
    if (pid == 0)
        return ERR_INVALID_PID;
    if (kill(pid, SIGCONT) < 0)
    {
        return ERR_INVALID_JOBNUM;
    }
    return 0;
}