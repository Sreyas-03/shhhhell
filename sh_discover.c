#include "headers.h"
#include "sh_discover.h"


char ogWD[MAX_LENGTH]={};
char foundWD[MAX_LENGTH]={};

static int cmp(const struct dirent** a, const struct dirent** b)
{
    return strcasecmp((*a)->d_name, (*b)->d_name);
}

static int processDir(char *path, DIR* dr)
{
    dr = opendir(".");
    if (dr == NULL)
    {
        return ERR_INVALID_PATH;
    }
    return 0;
}

static int printPath(char* fileName)
{
    char cwd[MAX_LENGTH];
    getcwd(cwd, MAX_LENGTH-1);
    strcat(cwd, "/");
    strcat(cwd, fileName);
    printf(".%s\n", cwd+strlen(ogWD));
}

static int discoBFS(int files, struct dirent** ent, char* fileName, int dFlag, int fFlag, int searchFileFlag)
{
    int retval = 1;

    for (int i=0; i<files; i++)
    {
        struct stat dstat;
        struct dirent* dd = ent[i];
        stat(dd->d_name, &dstat);
        if (searchFileFlag && !strcmp(dd->d_name, fileName))
        {
            getcwd(foundWD, MAX_LENGTH-1);
            printf(".%s\n", foundWD+strlen(ogWD));
            return 0;
        }
        else if (!searchFileFlag)
        {
            if (dFlag && !fFlag && S_ISDIR(dstat.st_mode) && dd->d_name[0]!='.')
                printPath(dd->d_name);
            else if (fFlag && !dFlag && !S_ISDIR(dstat.st_mode))
                printPath(dd->d_name);
            else if (((fFlag && dFlag) || (!fFlag && !dFlag)) && dd->d_name[0]!='.')
                printPath(dd->d_name);
        }
    }

    for (int i=0; i<files; i++)
    {
        struct stat dstat;
        struct dirent* dd = ent[i];
        stat(dd->d_name, &dstat);
        if (S_ISDIR(dstat.st_mode) && strcmp(dd->d_name, ".") && strcmp(dd->d_name, ".."))
        {
            // debug(dd->d_name);
            chdir(dd->d_name);
            struct dirent** entIn;
            DIR *dr;
            processDir(".", dr);
            int filesIn=0;
            if ((filesIn = scandir(".", &entIn, NULL, cmp)) == -1)
                return ERR_INVALID_PATH;

            retval = discoBFS(filesIn, entIn, fileName, dFlag, fFlag, searchFileFlag);
            chdir("..");
            if (!retval)
            {
                return 0;            
            }
        }
    }
    return retval;
}

static int print_disc(int fFlag, int dFlag, char* fileName)
{
    struct dirent** ent;
    DIR *dr;
    struct stat dstat;
    processDir(".", dr);
    int files=0;
    if ((files = scandir(".", &ent, NULL, cmp)) == -1)
    {
        return ERR_INVALID_PATH;
    }

    int fileSearchFlag = 1;
    if (!strlen(fileName))
        fileSearchFlag = 0;

    int retval = discoBFS(files, ent, fileName, dFlag, fFlag, fileSearchFlag);
    if (retval && fileSearchFlag)
    {
        return ERR_FILE_NOT_FOUND;
    }
    return 0;
}


int sh_discover(char* inputCmd)
{
    getcwd(ogWD, MAX_LENGTH-1);
    int dFlag = 0, fFlag = 0;
    int numArgs = 0, numFiles = 0;
    char** args = calloc(MAX_INPUT_CMDS, sizeof(char*));
    for(int i=0; i<MAX_INPUT_CMDS; i++)
        args[i] = calloc(MAX_FILENAME, sizeof(char));

    char dirName[MAX_FILENAME] = {};
    char fileName[MAX_FILENAME] = {};
        
    char *token;
    char *delimit = " \n\t\r\v\f";
    token = strtok(inputCmd, delimit);
    while (token != NULL)
    {
        strcpy(args[numArgs++], token);
        token = strtok(NULL, delimit);
    }

    for (int i = 0; i < numArgs; i++)
    {
        if (args[i][0] == '-')
        {
            if (args[i][1] == 'd' || args[i][2] == 'd')
                dFlag = 1;
            if (args[i][1] == 'f' || args[i][2] == 'f')
                fFlag = 1;
            if (!(args[i][1] == 'd' || args[i][2] == 'd') && !(args[i][1] == 'f' || args[i][2] == 'f'))
                return ERR_INVALID_FLAG;
        }
        else
        {
            if (args[i][0] == '"')
            {
                char* tempToken = strtok(args[i]+1, "\"");
                strcpy(fileName, tempToken);
            }
            else
            {
                strcpy(dirName, args[i]);
            }
            numFiles++;
        }
    }
    int retval = 0;
    if (strlen(dirName))
    {
        retval = chdir(dirName);
        if (retval < 0)
            return ERR_INVALID_PATH;
        retval = print_disc(fFlag, dFlag, fileName);
    }
    else
    {
        retval = print_disc(fFlag, dFlag, fileName);
    }
    return retval;
}