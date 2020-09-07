CC=gcc
output=main
test_output=test
files=main.c mylloc.c memmanager.c self_debugger.c 
test_files=unit_tests.c mylloc.c memmanager.c self_debugger.c 
debugger=gdb
flags=-g -O0 -lpthread -Wall

all:
	$(CC) $(flags) -o $(output) $(files)

run:
	./$(output)

debug:
	$(debugger) ./$(output)

test:
	$(CC) $(flags) -o $(test_output) $(test_files)
