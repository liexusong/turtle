CC=gcc
CFLAGS=-g -O3 -std=gnu99
CLFAGS_WARNING=-Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes #-Wconversion
LDFLAGS=-lfl
SOURCES= absyn.c dbg.c env.c instruction.c lexer.c main.c parser.c semant.c symbol.c table.c
HEADERS= absyn.h dbg.h env.h instruction.h lexer.h global.h parser.h semant.h symbol.h table.h
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=turtle

all: $(SOURCES) $(HEADER) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

lexer.o: lexer.c
	$(CC) $(CFLAGS) $< -c -o $@

lexer.c: lexer.l
	flex lexer.l

parser.c: parser.y
	bison -d parser.y

.c.o:
	$(CC) $(CFLAGS) $(CLFAGS_WARNING) $< -c -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)
