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
	END 0					"End of file"
	INDENT					"Indentation"
	DEDENT					"Unindentation"
	EQ						"="
	ADDEQ					"+="
	SUBEQ					"-="
	MULEQ					"*="
	DIVEQ					"/="
	INC						"++"
	DEC						"--"
	AUTO					"Auto type ($)"
	<string> ID				"Identifier"
	<string> INT			"Integer (int)"
	<string> NUM			"Float number (num)"
	<string> BOOL			"Boolean (bool)"
;

%type <Statement*>			stmt;
%type <Expression*>			expr;
%type <Literal*>			literal;
%type <UnaryOperation*>		unary_op;
%type <string>				type;
%type <string>				assign_op;

%start program

%%
program:
	| program stmt { compiler.statements.push_back($2); }
	;

stmt:
	type ID EQ expr { $$ = new VariableDeclaration($2, $1, $4); }
	| ID assign_op expr { $$ = new VariableAssignement($1, $3, $2); }
	| expr { $$ = new ExpressionStatement($1); }
	;

expr:
	literal { $$ = $1; }
	| unary_op { $$ = $1; }
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

assign_op:
	EQ { $$ = "="; }
	| ADDEQ { $$ = "+="; }
	| SUBEQ { $$ = "-="; }
	| MULEQ { $$ = "*="; }
	| DIVEQ { $$ = "/="; }
	;

unary_op:
	ID INC { $$ = new UnaryOperation($1, "++"); }
	| ID DEC { $$ = new UnaryOperation($1, "--"); }
	| INC ID { $$ = new UnaryOperation($2, "++", true); }
	| DEC ID { $$ = new UnaryOperation($2, "--", true); }
	;

%%

void Parser::error(const location &_, const string &msg)
{
	cerr << scanner.file << ":" <<
		scanner.loc.begin.line << ":" <<
		scanner.loc.begin.column << ": " <<
		msg << endl;
}
