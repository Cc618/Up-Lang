%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0"

%defines
%define api.value.type variant
%define api.namespace { up }
%define api.parser.class { Parser }
%define api.token.prefix {TOKEN_}
%define api.token.constructor
%define parse.assert
%define parse.trace
%define parse.error verbose

%locations

%param { Scanner &scanner }
%param { Compiler &compiler }

%code requires
{
	#include <iostream>
	#include <string>

	using namespace std;

	namespace up {
		class Scanner;
		class Compiler;
	}
}

%code top
{
	#include <iostream>

	#include "scanner.h"
	#include "parser.hpp"
	#include "compiler.h"

	using namespace up;

	// Replace the yylex function by up::Scanner::Next
	static inline Parser::symbol_type yylex(Scanner &scanner, Compiler &compiler)
	{ return scanner.next(); }
}

%token END 0 "End of file"
%token <string> LETTER  "Letter";
%token <int> NB "Number";

%type <int> number;

%start program

%%
program:
	| program number { compiler.printNumber($2); }
	| program LETTER { compiler.printLetter($2[0]); }
	;

number:
	NB LETTER { $$ = $1; }
	;
%%

void Parser::error(const location &loc, const string &msg)
{
	cerr << scanner.file << ":" << loc.begin.line << ":" << loc.begin.column
		<< ": " << msg << endl;
}
