CC=gcc
CFLAGS = -g
TARGET=exec #target file name

all:
	$(CC) $(CFLAGS) main.c sh_prompt.c sh_pwd.c sh_echo.c sh_error.c sh_cd.c sh_colours.c sh_history.c sh_ls.c sh_fg.c sh_bg.c sh_pinfo.c sh_discover.c sh_redir.c sh_jobs.c sh_justBG.c sh_justFG.c sh_signal.c sh_autoComplete.c -o $(TARGET)

clean:
	rm *.o $(TARGET)
