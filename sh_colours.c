#include "headers.h"
#include "sh_colours.h"

int sh_green(char* text)
{
    printf("\033[0;32m%s\033[0m", text);
    return 0;
}

int sh_blue(char* text)
{
    printf("\033[0;34m%s\033[0m", text);
    return 0;
}

int sh_red(char* text)
{
    printf("\033[0;31m%s\033[0m", text);
    return 0;
}

int sh_white(char* text)
{
    printf("\033[0m%s", text);
    return 0;
}