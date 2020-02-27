PARSER ?= bison
LEXER ?= flex

PARSER_ARGS ?= --report=state
CPP_ARGS ?= -std=c++17

.PHONY: all clean

# Compiles the program bin/up (the bin directory must be created)
all: lexer.cpp parser.cpp
	g++ $(CPP_ARGS) -o ../bin/up *.cpp

lexer.cpp: lexer.l
	$(LEXER) -o lexer.cpp lexer.l

parser.cpp: parser.y
	$(PARSER) $(PARSER_ARGS) -o parser.cpp parser.y

clean:
	rm -rf lexer.cpp
	rm -rf parser.cpp parser.hpp location.hh position.hh stack.hh parser.output
