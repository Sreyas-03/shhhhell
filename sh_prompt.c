#include "headers.h"
#include "sh_prompt.h"
#include "sh_colours.h"

int promptData(char* userName, char* systemName, char* wd, int execTime)
{
    if (execTime > 1)
    {
        printf("\r");
    }
    sh_green("<");
    sh_green(userName);
    sh_green("@");
    sh_green(systemName);
    printf(":");
    sh_blue(wd);
    if(execTime>1)
    {
        printf("%s took %ds\033[0m", "\x1b[36m", execTime);
    }
    sh_green("> ");
    
    return 0;
}