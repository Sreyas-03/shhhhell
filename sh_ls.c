#include "headers.h"
#include "sh_ls.h"
#include "sh_error.h"
#include "sh_colours.h"

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

static int print_ls(int aFlag, int lFlag)
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

    for (int i=0; i<files; i++)
    {
        struct dirent* dd = ent[i];
        if (!aFlag && dd->d_name[0]=='.')
            continue;

        stat(dd->d_name, &dstat);
        char props[11];
        props[0] = (S_ISDIR(dstat.st_mode)) ? 'd' : '-';
        props[1] = (dstat.st_mode & S_IRUSR) ? 'r' : '-';
        props[2] = (dstat.st_mode & S_IWUSR) ? 'w' : '-';
        props[3] = (dstat.st_mode & S_IXUSR) ? 'x' : '-';
        props[4] = (dstat.st_mode & S_IRGRP) ? 'r' : '-';
        props[5] = (dstat.st_mode & S_IWGRP) ? 'w' : '-';
        props[6] = (dstat.st_mode & S_IXGRP) ? 'x' : '-';
        props[7] = (dstat.st_mode & S_IROTH) ? 'r' : '-';
        props[8] = (dstat.st_mode & S_IWOTH) ? 'w' : '-';
        props[9] = (dstat.st_mode & S_IXOTH) ? 'x' : '-';
        props[10] = '\0';
        struct group *group;
        struct passwd *owner;
        struct tm *time = localtime(&dstat.st_mtime);

        if (lFlag)
        {
            printf("%s  ", props);
            owner = getpwuid(dstat.st_uid);
            group = getgrgid(dstat.st_gid);
            char curr_time[DATE_TIME_LENGTH];

            printf("%2lu  %s ", dstat.st_nlink, owner->pw_name);
            strftime(curr_time, sizeof(curr_time), "%b", time);
            printf("%s  %6ld ", group->gr_name, dstat.st_size);
            printf(" %s %2d  %02d:%02d ", curr_time, time->tm_mday, time->tm_hour, time->tm_min);
        }

        if (props[0]=='d')
        {
            printf("\033[1;34m%s   \033[0m", dd->d_name);
        }
        else
        {
            if (props[0]=='-' && (props[3]=='x' || props[6]=='x' || props[9]=='x'))
                printf("\033[1;32m%s   \033[0m", dd->d_name);
            else
                printf("%s   ", dd->d_name);
        }
        if (lFlag)
            printf("\n");
    }
    printf("\n");
    return 0;
}


int sh_ls(char *input, char* pwd)
{
    if (input == NULL)
        input = ".";

    int aFlag = 0, lFlag = 0;
    int numArgs = 0, numFiles = 0;
    char** args = calloc(MAX_INPUT_CMDS, sizeof(char*));
    for(int i=0; i<MAX_INPUT_CMDS; i++)
    {
        args[i] = calloc(MAX_FILENAME, sizeof(char));
    }

    char *token;
    char *delimit = " \n\t\r\v\f";
    token = strtok(input, delimit);

    while (token != NULL)
    {
        strcpy(args[numArgs++], token);
        token = strtok(NULL, delimit);
    }

    for (int i = 0; i < numArgs; i++)
    {
        if (args[i][0] == '-')
        {
            if (args[i][1] == 'a' || args[i][2] == 'a')
                aFlag = 1;
            if (args[i][1] == 'l' || args[i][2] == 'l')
                lFlag = 1;
            if (!(args[i][1] == 'a' || args[i][2] == 'a') && !(args[i][1] == 'l' || args[i][2] == 'l'))
                return ERR_INVALID_FLAG;
        }
        else
        {
            numFiles++;
        }
    }

    if (numFiles==0)
    {
        int retval = print_ls(aFlag, lFlag);
        return retval;
    }

    for (int i = 0; args[i] != NULL; i++)
    {
        char temp[MAX_FILENAME];
        strcpy(temp, args[i]);
        char* tempToken = strtok(temp, delimit);
        if (tempToken == NULL)
            continue;
            
        char currWD[MAX_FILENAME-1] = {};
        strcpy(currWD, pwd);
        if (args[i][0] == '-')
            continue;


        struct stat dstat;
        stat(args[i], &dstat);
        if (!(S_ISDIR(dstat.st_mode)))
        {
            if ((dstat.st_mode & S_IXUSR) || (dstat.st_mode & S_IXGRP) || (dstat.st_mode & S_IXOTH))
                printf("\033[1;32m%s   \033[0m\n", args[i]);
            else
                printf("%s   \n", args[i]);
            
            continue;
        }

        

        if (args[i][0] == '/')
            strcat(currWD, args[i]);
        else
        {
            strcat(currWD, "/");
            strcat(currWD, args[i]);
        }

        if (numFiles > 1)
            printf("%s\n", args[i]);

        chdir(currWD);
        char currPWD[MAX_LENGTH];
        getcwd(currPWD, MAX_LENGTH);

        int retval = print_ls(aFlag, lFlag);
        if (retval)
            return retval;
        chdir(pwd);
    }

    for(int i=0; i<MAX_INPUT_CMDS; i++)
    {
        free(args[i]);
    }
    free(args);

    return 0;
}