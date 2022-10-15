#include "headers.h"
#include "sh_cd.h"

static int changeDir(char* path)
{
    if (chdir(path) < 0)
    {
        return ERR_INVALID_DIRECTORY_CD;
    }
    return 0;
}

int sh_cd(char* args)
{
    char path[MAX_LENGTH];
    char realPWD[MAX_LENGTH];
    char OGpwd[MAX_LENGTH];
    getcwd(realPWD, MAX_LENGTH-1);
    getcwd(OGpwd, MAX_LENGTH-1);
    
    char* delimiter = " \n\t\r\v\f";
    char* tempChk;
    tempChk = strtok(args, delimiter);
    
    if (tempChk!= NULL)
    {
        strcpy(path, tempChk);
        tempChk = strtok(NULL, delimiter);
        if (tempChk != NULL)
        {
            return ERR_EXTRA_PARAMETER;
        }
    }
    else
    {
        return 0;
    }

    if (path[0] == '.')
    {
        strcpy(realPWD, path);
    }

    else if (path[0]=='/' && strlen(path)==1)
    {
        strcpy(realPWD, home);
        strcat(realPWD, "/../../../../../../../../..");
    }

    else if (path[0]=='/')
    {
        strcpy(realPWD, path+0);
    }

    else if (path[0] == '~')
    {
        strcpy(realPWD, home);
        strcat(realPWD, path+0+1);
    }

    else if (path[0] == '-')
    {
        debug(prev_dir);
        strcpy(realPWD, prev_dir);
    }

    else
    {
        strcat(realPWD, "/");
        strcat(realPWD, path+0);
    }

    strcpy(prev_dir, OGpwd);
    int retval = changeDir(realPWD);
    if (retval)
    {
        changeDir(OGpwd);
        return retval;
    }

    getcwd(realPWD, MAX_LENGTH-1);
    if (strncmp(realPWD, home, strlen(home)))
    {
        chdir(realPWD);
        return 0;
    }

    // strcpy(prev_dir, OGpwd);
    return retval;
}