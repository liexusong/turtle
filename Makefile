CC=clang
CFLAGS=-Wall -Wextra -g -O3
LDFLAGS=-lfl
SOURCES= absyn.c dbg.c env.c instruction.c lexer.c main.c parser.c semant.c symbol.c table.c
HEADERS= absyn.h dbg.h env.h instruction.h lexer.h global.h parser.h semant.h symbol.h table.h
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=turtle

all: $(SOURCES) $(HEADER) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

lexer.c: lexer.l
	flex lexer.l

parser.c: parser.y
	bison -d parser.y

.c.o:
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm $(OBJECTS)
	rm $(EXECUTABLE)
