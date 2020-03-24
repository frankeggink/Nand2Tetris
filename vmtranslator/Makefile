CC=gcc
CFLAGS = -std=c99 -Wall -Wextra -g
DEPS = codewriter_hack.h filehelper.h parser.h processhelper.h stringhelper.h
OBJ = main.o codewriter_hack.o filehelper.o parser.o processhelper.o stringhelper.o

all: VMTranslator

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

VMTranslator: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	chmod +x VMTranslator
	
.PHONY: clean

clean: 
	rm -rf *.o VMTranslator	
