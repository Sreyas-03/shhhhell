#include "headers.h"
#include "sh_signal.h"
#include "sh_pinfo.h"

int findBGJobNum(int pid)
{
    for(int i=0; i<MAX_BG_PROCESS; i++)
    {
        if (bgPID[i] == pid)
            return i;
    }
    return -1;
}

int sh_signal(int pid, int sig_Num)
{
    int status;
    deb(pid);
    int retval = 0;
    if (!pid)
        return ERR_INVALID_JOBNUM;

    status = sig_Num;
    if (status == 1||status == 8||status == 9||status == 10)
    {
        int jobNum = findBGJobNum(pid);
        if (jobNum==-1)
            return ERR_INVALID_JOBNUM;
        bgPID[jobNum] = 0;
        bgProc[jobNum] = NULL;
    }

    retval = kill(pid, sig_Num);

    if (retval < 0)
    {
        return ERR_INVALID_JOBNUM;
    }
    
    return 0;
}
