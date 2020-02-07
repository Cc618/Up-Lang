
.PHONY: all clean

# Compiles the program bin/up (the bin directory must be created)
all: lexer.cpp parser.cpp
	g++ -o ../bin/up *.cpp

lexer.cpp: lexer.l
	flex -o lexer.cpp lexer.l

parser.cpp: parser.y
	bison -o parser.cpp parser.y

clean:
	rm -rf lexer.cpp
	rm -rf parser.cpp parser.hpp location.hh position.hh stack.hh
