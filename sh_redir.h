#ifndef __REDIR_H__
#define __REDIR_H__

// int findPipedCmds(char inpCmd[MAX_LENGTH]);
int sh_redirPipe(char inpCmd[MAX_LENGTH], int inDesc, int outDesc);
int sh_redir(char inpCmd[MAX_LENGTH]);
int sh_standardizeInOut();

#endif