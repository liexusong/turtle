CC=clang
CFLAGS=-Wall -Wextra -g -O3
LDFLAGS=-lfl
SOURCES= absyn.c dbg.c env.c instruction.c lexer.c main.c parser.c semant.c symbol.c table.c
OBJECTS=$(SOURCES:.h=.o)
EXECUTABLE=turtle

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

lexer.c: lexer.l
	flex lexer.l

parser.c: parser.y
	bison parser.y

.c.o:
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
