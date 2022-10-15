#include "headers.h"
#include "sh_history.h"

int getHist()
{
    FILE *ifp = fopen(".history.data", "rb");
    if (ifp == NULL)
    {
        FILE* f = fopen(".history.data", "wb");
        fclose(f);
        fclose(ifp);
        return 0;
    }
    fread(&historyStart, sizeof(int), sizeof(historyStart), ifp); 
    fread(history, sizeof(char), sizeof(history), ifp);
    return 0;
}

int storeHist()
{
    FILE* f = fopen(".history.data", "wb");
    fwrite(&historyStart, sizeof(int), sizeof(historyStart), f);
    fwrite(history, sizeof(char), sizeof(history), f);
    fclose(f);
    return 0;
}

static int println(char* printLine)
{
    int cnt=0;
    char c = printLine[cnt++];
    while (c != '\n' && c!='\0')
    {
        printf ("%c", c);
        c = printLine[cnt++];
    }
    printf("\n");
    return 0;
}

int makeHistory(char* text)
{
    if (!strcmp((history[(historyStart-1)%HISTORY_LENGTH]), text))
        return 0;
    strcpy(history[(historyStart%HISTORY_LENGTH)], text);
    historyStart++;
    return 0;
}

int sh_history(char* args)
{
    int numPrint = 10;
    if (args != NULL)
    {
        char* token;
        char delimit[] = " \n\t\r\v\f";
        token=strtok(args, delimit);
        int inputNum = atoi(token);

        token=strtok(NULL, delimit);
        if (inputNum == 0 || token != NULL)
        {
            return ERR_EXTRA_PARAMETER;
        }
        numPrint = (HISTORY_LENGTH<inputNum)?HISTORY_LENGTH:inputNum+1;
    }

    numPrint = (historyStart<numPrint)?historyStart:numPrint;
    for(int i=0; i<numPrint-1; i++)
    {
        println(history[(historyStart-numPrint+i)%HISTORY_LENGTH]);
    }
    return 0;
}
