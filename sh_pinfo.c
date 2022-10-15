#include "headers.h"
#include "sh_pinfo.h"

int sh_pinfo(char *inpPID)
{
    char status;
    int pid, mem, pgrp = -2, tpgid = -1;
    char execPath[MAX_LENGTH];
    char fileName[MAX_FILENAME];
    sprintf(fileName, "/proc/%s/stat", inpPID);
    FILE *fstat = fopen(fileName, "r");
    if (fstat == NULL)
        return ERR_INVALID_PID;

    fscanf(fstat, "%d %*s %c %*s %d %*s %*s %d %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %d", &pid, &status, &pgrp, &tpgid, &mem);
    printf("pinfo : %d\n", pid);
    printf("process status : %c%c\n", status, (pgrp==tpgid) ? '+' : ' ');
    printf("memory : %d\n", mem);
    fclose(fstat);

    sprintf(fileName, "/proc/%s/exe", inpPID);
    int retval = readlink(fileName, execPath, 1023);
    if (retval == -1)
        return ERR_INVALID_PID;
    execPath[retval] = '\0';

    char temp[MAX_LENGTH-2];
    if (!strncmp(execPath, home, strlen(home)))
    {
        strcpy(temp, execPath+strlen(home));
        sprintf(execPath, "~%s", temp);
    }

    debug(execPath);
    return 0;
}