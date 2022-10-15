#include "headers.h"
#include "sh_error.h"

int sh_error(char* cmd, int errorID)
{
    if (errorID == ERR_EXTRA_PARAMETER)
    {
        printf("%s: too many arguments\n", cmd);
        printf("Try 'man %s' for more information about the command\n", cmd);
    }

    else if (errorID == ERR_MISSING_PARAMETER)
    {
        printf("%s: too few parameters\n", cmd);
        printf("Try 'man %s' for more information about the command\n", cmd);
    }

    else if (errorID == ERR_INVALID_CMD)
    {
        printf("command not found: %s\n", cmd);
    }

    else if (errorID == ERR_INVALID_DIRECTORY_CD)
    {
        printf("cd: string not in pwd: %s\n", cmd); // cmd is the invalid directory
    }

    else if (errorID == ERR_INVALID_PARAMETER)
    {
        printf("%s: parameter should be non-zero integer\n", cmd);
    }

    else if (errorID == ERR_INVALID_PATH)
    {
        printf("%s: invalid path entered\n", cmd);
    }

    else if (errorID == ERR_INVALID_FLAG)
    {
        printf("%s: invalid flag for %s\n", cmd, cmd);
        printf("Try 'man %s' for more information about the flags for %s\n", cmd, cmd);
    }
    
    else if (errorID == ERR_BG_PROCESS)
    {
        printf("%s: unable to start background process. Try again\n", cmd);
    }

    else if (errorID == ERR_PROCESS)
    {
        printf("%s: unable to start process. Try again\n", cmd);
    }

    else if (errorID == ERR_FILE_OPEN)
    {
        printf("%s: unable to open file. Try again\n", cmd);
    }

    else if (errorID == ERR_INVALID_PID)
    {
        printf("%s: The PID entered doesn't exist.\n Try 'ps' to see the running processes\n", cmd);
    }

    else if (errorID == ERR_INVALID_LOGIN)
    {
        printf("%s: Unable to login. Try again\n", cmd);
    }
    
    else if (errorID == ERR_FILE_NOT_FOUND)
    {
        printf("%s: Unable to locate file. Try again\n", cmd);
    }

    else if (errorID == ERR_FILE_DNE)
    {
        printf("%s: Input file not found. Enter a valid file\n", cmd);
    }
    return 0;
}