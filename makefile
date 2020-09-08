CC=gcc
output=main
files=main.c mylloc.c memmanager.c self_debugger.c unit_tests.c
debugger=gdb
flags=-g -O0 -pthread -Wall

all:
	$(CC) $(flags) -o $(output) $(files)

run:
	./$(output)

debug:
	$(debugger) ./$(output)

update:
	git pull

quick_push:
	git add *
	git commit -m 'quick update'
	git push