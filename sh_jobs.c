#include "headers.h"
#include "sh_jobs.h"

int jobs[MAX_BG_PROCESS];

static int cmpfun(const void* a, const void* b)
{
    return strcmp(bgProc[*(int*)a], bgProc[*(int*)b]);
}

static int sh_pjobs(int inpPID, int jobNum, int rFlag, int sFlag)
{
    char status;
    int pid, mem, pgrp = -2, tpgid = -1;
    char fileName[MAX_FILENAME];
    sprintf(fileName, "/proc/%d/stat", inpPID);
    FILE *fstat = fopen(fileName, "r");
    if (fstat == NULL)
        return ERR_INVALID_PID;

    fscanf(fstat, "%d %*s %c %*s %d %*s %*s %d %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %d", &pid, &status, &pgrp, &tpgid, &mem);
    
    if (status == 'T' && sFlag)
        printf("[%d]  %s  %s  [%d]\n", jobNum+1, "Stopped", bgProc[jobNum], inpPID);
    
    else if (status != 'T' && rFlag)
        printf("[%d]  %s  %s  [%d]\n", jobNum+1, "Running", bgProc[jobNum], inpPID);
    
    fclose(fstat);
}

int sh_jobs(char* flags)
{
    int sFlag = 0, rFlag = 0;
    int numArgs = 0;
    char** args = calloc(MAX_INPUT_CMDS, sizeof(char*));
    for(int i=0; i<MAX_INPUT_CMDS; i++)
    {
        args[i] = calloc(MAX_FILENAME, sizeof(char));
    }

    char *token;
    char *delimit = " \n\t\r\v\f";
    token = strtok(flags, delimit);

    while (token != NULL)
    {
        strcpy(args[numArgs++], token);
        token = strtok(NULL, delimit);
    }

    for (int i = 0; i < numArgs; i++)
    {
        if (args[i][0] == '-')
        {
            if (args[i][1] == 'r' || args[i][2] == 'r')
                rFlag = 1;
            if (args[i][1] == 's' || args[i][2] == 's')
                sFlag = 1;
            if (!(args[i][1] == 's' || args[i][2] == 's') && !(args[i][1] == 'r' || args[i][2] == 'r'))
                return ERR_INVALID_FLAG;
        }
    }
    if (sFlag == 0 && rFlag == 0)
    {
        sFlag = 1;
        rFlag = 1;
    }


    int numJobs=0;
    for (int i=0; i<MAX_BG_PROCESS; i++)
    {
        if (bgPID[i])
            jobs[numJobs++] = i;
    }
    qsort(jobs, numJobs, sizeof(int), cmpfun);
    
    for(int i=0; i<numJobs; i++)
    {
        sh_pjobs(bgPID[jobs[i]], jobs[i], rFlag, sFlag);   
    }
    return 0;
}