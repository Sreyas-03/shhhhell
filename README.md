# OSN Assignment-2 (Shell)
## 2021111016 | S.Sreyas
___
___
## Requirements
- GCC compiler
- Linux OS (preferably Ubuntu)

## To start the shell - 
- cd into the directory of the project and enter `make` followed by `./exec`

- give execute permission to exec file (if it doesnt already have it)

## Feature and Files-
This shell has most of the basic functions that a normal shell has
- main.c
    - handles the parsing of instructions
    - calls the appropriate functions for executing the command
    - initializes the value of the global variables
    - runs the infinite shell loop

- sh_pwd.c
    - fetches and prints the current working directory of the shell

- sh_prompt.c
    - displays the default prompt of the shell
    
- sh_bg.c
    - creates a new process
    - runs the background process

- sh_cd.c
    - changes the working directory of the shell

- sh_colours.c
    - prints coloured outputs on the shell

- sh_discover.c
    - recursively traverses folders to search for files
    - equivalent to the basic functionalities of find command

- sh_echo.c
    - prints the string onto the shell  
    - truncates all whitespaces to at most one

- sh_error.c
    - handles most of the common errors that may occur during execution
    - prompts the user with details about the error

- sh_fg.c
    - runs a process in the foreground
    - any in-built command that is not built in this shell is executed in the foreground (unless forced to be done in background)

- sh_history.c
    - stores the history of commands called by the user
    - stores information across sessions of the shell
    - takes input from user as to the number of values printed (10 by default)

- sh_ls.c
    - lists out the files in the given directory
    - handles -a and -l flags
    - colour codes the filenames based on their properties

- sh_redir.c
    - redirects input and output from given files
    - pipes information between processes

- sh_justfg.c
    - converts a bg process into fg

- sh_justbg.c
    - continues a stopped bg process

- sh_jobs.c
    - lists the background processes and their status lexicographically

- sh_autocomplete.c
    - completes the filename on pressing tab

- sh_signal.c
    - sends signal to the given process


## List of assumptions-
- Reasonable assumptions have been made for the maximum length of commands, filenames etc.
- quotes are not handled in echo
- directory names having spaces havent been handled
- echo doesnt handle environmental variables
- a hidden file *.history.data* has been created to store historic information. Tampering the file could potentially damage the working of history function
- Precedence order followed - **" ; > & > other characters, strings"**
Therfore echo cant print ";", "|" and "&" symbols
