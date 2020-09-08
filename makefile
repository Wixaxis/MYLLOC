CC=gcc
output=main
test_output=test
files=main.c mylloc.c memmanager.c self_debugger.c 
test_files=unit_tests.c mylloc.c memmanager.c self_debugger.c 
no_self_debug_files=unit_tests.c mylloc.c memmanager.c
debugger=gdb
flags=-g -O0 -pthread -Wall

all:
	$(CC) $(flags) -o $(output) $(files)

run:
	./$(output)

debug:
	$(debugger) ./$(output)

test:
	$(CC) $(flags) -o $(test_output) $(test_files)

noselfdebug:
	$(CC) $(flags) -o $(test_output) $(no_self_debug_files)

