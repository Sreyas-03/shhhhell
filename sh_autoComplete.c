#include "headers.h"
#include "sh_autoComplete.h"
#include "sh_error.h"

static int cmp(const struct dirent** a, const struct dirent** b)
{
    return strcasecmp((*a)->d_name, (*b)->d_name);
}

static int processDir(char *path, DIR* dr)
{
    dr = opendir(".");
    if (dr == NULL)
    {
        sh_error("ls", ERR_INVALID_PATH);
        return ERR_INVALID_PATH;
    }
    return 0;
}


int sh_autoComplete(char* inp, int* pt)
{
    char inpCopy[MAX_LENGTH]={};
    char lastInp[MAX_LENGTH]={};
    strcpy(inpCopy, inp);

    if (inp[*pt-1] == ' ')
    {
        return 0;
    }

    char* token;
    char delimit[] = " \n\t\r\v\f";
    token = strtok(inpCopy, delimit);

    while(token != NULL)
    {
        strcpy(lastInp, token);
        token = strtok(NULL, delimit);
    }

    char* possibleFiles[MAX_INPUT_CMDS];
    int numPossibleFiles = 0;
    struct dirent** ent;
    DIR *dr;
    struct stat dstat;
    processDir(".", dr);
    int files=0;
    if ((files = scandir(".", &ent, NULL, cmp)) == -1)
    {
        return ERR_INVALID_PATH;
    }

    for (int i=0; i<files; i++)
    {
        struct dirent* dd = ent[i];
        stat(dd->d_name, &dstat);
        int props = (S_ISDIR(dstat.st_mode));
        if (!strncmp(dd->d_name, lastInp, strlen(lastInp)))
        {
            possibleFiles[(numPossibleFiles++)%MAX_INPUT_CMDS] = dd->d_name;
        }
    }

    if (numPossibleFiles == 0)
    {
        return 0;
    }
    
    else if (numPossibleFiles == 1)
    {
        for(int i=strlen(lastInp); i<strlen(possibleFiles[0]); i++)
        {
            inp[(*pt)++] = possibleFiles[0][i];
        }
        stat(possibleFiles[0], &dstat);
        int props = (S_ISDIR(dstat.st_mode));
        if (props)
            inp[(*pt)++] = '/';
    }

    else
    {
        numPossibleFiles = numPossibleFiles%MAX_INPUT_CMDS;
        printf("\n");
        char commonPart[MAX_LENGTH];
        strcpy(commonPart, possibleFiles[0]);

        for(int i=0; i<numPossibleFiles; i++)
        {
            printf("%s\n", possibleFiles[i]);
            int minLen = strlen(commonPart);
            if (strlen(possibleFiles[i]) < minLen)
                minLen = strlen(possibleFiles[i]);
            for(int j=0; j<minLen; j++)
            {
                if (commonPart[j] != possibleFiles[i][j])
                {
                    commonPart[j] = '\0';
                    break;
                }
            }
        }
        
        for(int i=strlen(lastInp); i<strlen(commonPart); i++)
            inp[(*pt)++] = commonPart[i];

        return 1;
    }
    return 0;
}