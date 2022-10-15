#include "headers.h"
#include "sh_redir.h"
#include "sh_error.h"

static int changeStandard()
{
    if (newIn)
    {
        OGstdin = dup(STDIN_FILENO);
        dup2(newIn, STDIN_FILENO);
    }
    if (newOut && (newOut-1))
    {
        OGstdout = dup(STDOUT_FILENO);
        dup2(newOut, STDOUT_FILENO);
    }
    return 0;
}


int sh_standardizeInOut()
{
    if(newIn)
    {
        close(newIn);
        dup2(OGstdin, 0);
        close(OGstdin);
        newIn = 0;
    }

    if(newOut && (newOut-1))
    {
        close(newOut);
        dup2(OGstdout, 1);
        close(OGstdout);
        newOut = 1;
    }
    return 0;
}


int sh_redir(char inpCmd[MAX_LENGTH])
{
    char modInput[MAX_LENGTH]={};
    char inpCpy[MAX_LENGTH];
    strcpy(inpCpy, inpCmd);
    int inFlag=0, outFlag=0;
    char *delimit = " \n\t\r\v\f";

    char *token = strtok(inpCpy, delimit);
    while (token != NULL)
    {
        if (inFlag>0)
        {
            inFlag = 0;
            newIn = open(token, O_RDONLY);
            if (newIn < 0)
            {
                newIn = 0;
                newOut = 0;
                sh_standardizeInOut();
                sh_error(token, ERR_FILE_DNE);
                return ERR_FILE_DNE;
            }
        }
        else if (outFlag == 1)
        {
            outFlag = 0;
            newOut = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        else if (outFlag == 2)
        {
            outFlag = -1;
            newOut = open(token, O_WRONLY | O_CREAT | O_APPEND, 0644);
        }
        else if (!strcmp(token, "<"))
            inFlag = 1;
        else if (!strcmp(token, ">"))
            outFlag = 1;
        else if (!strcmp(token, ">>"))
            outFlag = 2;
        else if (inFlag == 0 && outFlag == 0)
        {
            strcat(modInput, token);
            strcat(modInput, " ");
        }
        
        token = strtok(NULL, delimit);
    }
    strcpy(inpCmd, modInput);
    changeStandard();
    return 0;
}
