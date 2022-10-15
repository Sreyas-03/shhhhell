#include "headers.h"
#include "sh_echo.h"
#include "sh_redir.h"

int sh_echo(char* printText)
{
    char delimit[] = " \n\t\r\v\f";
    char* token = strtok(printText, delimit);
    while (token!=NULL)
    {
        printf("%s ", token);
        token=strtok(NULL, delimit);
    }
    printf("\n");
    sh_standardizeInOut();
    return 0;
}
