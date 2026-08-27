CC = gcc
CFLAGS = -ansi -Wall -pedantic

EXEC = assembler

# Added error.o to the objects list
OBJS = main.o utils.o symbol_table.o parser.o macro.o image.o pass1.o pass2.o output_files.o error.o

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

main.o: main.c globals.h image.h symbol_table.h pass1.h pass2.h output_files.h macro.h parser.h utils.h error.h
	$(CC) $(CFLAGS) -c main.c

# Rule for the new error module
error.o: error.c error.h globals.h
	$(CC) $(CFLAGS) -c error.c

utils.o: utils.c utils.h macro.h globals.h error.h
	$(CC) $(CFLAGS) -c utils.c

symbol_table.o: symbol_table.c symbol_table.h globals.h utils.h error.h
	$(CC) $(CFLAGS) -c symbol_table.c

parser.o: parser.c parser.h globals.h utils.h image.h error.h
	$(CC) $(CFLAGS) -c parser.c

macro.o: macro.c macro.h globals.h utils.h error.h
	$(CC) $(CFLAGS) -c macro.c

image.o: image.c image.h error.h
	$(CC) $(CFLAGS) -c image.c

pass1.o: pass1.c pass1.h globals.h symbol_table.h parser.h utils.h macro.h image.h error.h
	$(CC) $(CFLAGS) -c pass1.c

pass2.o: pass2.c pass2.h globals.h symbol_table.h parser.h output_files.h image.h error.h
	$(CC) $(CFLAGS) -c pass2.c

output_files.o: output_files.c output_files.h image.h symbol_table.h globals.h error.h
	$(CC) $(CFLAGS) -c output_files.c

clean:
	rm -f *.o $(EXEC) *.am *.ob *.ext *.ent
