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
	#include "components.h"

	using namespace up;

	// Replace the yylex function by up::Scanner::Next
	static inline Parser::symbol_type yylex(Scanner &scanner, Compiler &compiler)
	{ return scanner.next(); }
}

%token
	END 0 "End of file"
	INDENT "Indentation"
	DEDENT "Unindentation"
	EQ "="
	AUTO "Auto type ($)"
	<string> ID "Identifier"
	<string> INT "Integer (int)"
	<string> NUM "Float number (num)"
	<string> BOOL "Boolean (bool)"
;

%type <Literal*> literal;
%type <string> type;

%start program

%%
program:
	| program type ID EQ literal { compiler.statements.push_back(new VariableDeclaration($3, $2, $5)); }
	/* | program ID EQ literal { compiler.assignVariable($2, $3, $5); } */
	;

literal:
	INT { $$ = new Literal($1, "int"); }
	| NUM { $$ = new Literal($1, "num"); }
	| BOOL { $$ = new Literal($1, "bool"); }
	;

type:
	ID { $$ = $1; }
	| AUTO { $$ = "auto"; }
	;
%%

void Parser::error(const location &_, const string &msg)
{
	cerr << scanner.file << ":" <<
		scanner.loc.begin.line << ":" <<
		scanner.loc.begin.column << ": " <<
		msg << endl;
}
