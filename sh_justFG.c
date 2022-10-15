#include "headers.h"
#include "sh_justFG.h"

int sh_justFG(int pid)
{
    int status;
    // setpgid(pid, getpgid(0));

    // signal(SIGTTIN, SIG_IGN);
    // signal(SIGTTOU, SIG_IGN);

    // tcsetpgrp(0, pid);

    // if (kill(pid, SIGKILL) < 0)
    //     perror("Error");
    clock_gettime(CLOCK_REALTIME, &start);
    fgRunning = pid;
    setpgid(pid, getpgid(0));
    waitpid(pid, &status, 0);
    clock_gettime(CLOCK_REALTIME, &finish);
    time_taken = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000 - 0.05;

    // deb(status);
    // tcsetpgrp(0, getpgid(0));

    // signal(SIGTTIN, SIG_DFL);
    // signal(SIGTTOU, SIG_DFL);

    if (WIFSTOPPED(status))
        return 0;

    return WIFEXITED(status) ? (int)time_taken : ERR_INVALID_JOBNUM;
}