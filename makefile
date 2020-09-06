CC=gcc
output=main
files=main.c mylloc.c memmanager.c self_debugger.c
debugger=gdb
flags=-g -O0 -lpthread -Wall

all:
	$(CC) $(flags) -o $(output) $(files)

run:
	./$(output)

debug:
	$(debugger) ./$(output)