CC=gcc
CFLAGS=-g -O3 -std=gnu99 -DNDEBUG
CLFAGS_WARNING=-Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes#-Wconversion
LDFLAGS=-lfl
SOURCES= absyn.c dbg.c env.c instruction.c lexer.c main.c parser.c semant.c symbol.c table.c
HEADERS= absyn.h dbg.h env.h instruction.h lexer.h global.h parser.h semant.h symbol.h table.h
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=turtle
DISASM=tools/DisASM
DISASMHS=tools/DisASM.hs

all: $(SOURCES) $(HEADER) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(CLFAGS_WARNING) $(OBJECTS) -o $@ $(LDFLAGS)

lexer.o: lexer.c
	$(CC) $(CFLAGS) $< -c -o $@

lexer.c: lexer.l
	flex lexer.l

parser.c: parser.y
	bison -d parser.y

.c.o:
	$(CC) $(CFLAGS) $(CLFAGS_WARNING) $< -c -o $@

test: $(DISASM)
	./run_tests.sh

$(DISASM) : $(DISASMHS)
	ghc -o $(DISASM) $(DISASMHS)

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)
