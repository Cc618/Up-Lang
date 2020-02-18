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
	#include "module.h"

	using namespace up;

	// Replace the yylex function by up::Scanner::Next
	static inline Parser::symbol_type yylex(Scanner &scanner, Compiler &compiler)
	{ return scanner.next(); }

	// Used for statements
	#define ERROR_INFO scanner.errorInfo()
}

%token
	END 0					"End of file"
	NL						"New line"
	INDENT					"Indentation"
	DEDENT					"Unindentation"
	EQ						"="
	ADDEQ					"+="
	SUBEQ					"-="
	MULEQ					"*="
	DIVEQ					"/="
	INC						"++"
	DEC						"--"
	PAR_BEGIN				"("
	PAR_END					")"
	COMMA					","
	AUTO					"Auto type ($)"
	USE						"use keyword"
	CDEF					"cdef keyword"
	<string> ID				"Identifier"
	<string> INT			"Integer (int)"
	<string> NUM			"Float number (num)"
	<string> BOOL			"Boolean (bool)"
	<string> STR			"String (str)"
;

%type <Statement*>			stmt;
%type <Expression*>			expr;
%type <Literal*>			literal;
%type <UnaryOperation*>		unary_op;
%type <Call*>				call;
%type <Call*>				call_start;
%type <Block*>				block;
%type <Block*>				block_start;
%type <Module>				import;
%type <string>				assign_op;
%type <char>				new_line;

%start program

%%
program:
	| program stmt new_line { compiler.main()->statements.push_back($2); }
	| program import { compiler.import($2); }
	| program new_line {}
	/* TODO */
	/* | program block { cout << "BLOCK\n" << $2->toString(); } */
	// | program new_line
	;


/*
block:
	block_start DEDENT { $$ = $1; }
	;

block_start:
	INDENT stmt { cout << "ok\n"; $$ = new Block(); $$->statements.push_back($2); }
	| INDENT AUTO { $$ = nullptr; cout << "ok\n"; }
	| block_start stmt { $$ = $1; $$->statements.push_back($2); }
	;
*/



stmt:
	ID ID EQ expr { $$ = new VariableDeclaration(ERROR_INFO, $2, $1, $4); }
	| AUTO ID EQ expr { $$ = new VariableDeclaration(ERROR_INFO, $2, "auto", $4); }
	| ID ID { $$ = new VariableDeclaration(ERROR_INFO, $2, $1, nullptr); }
	| CDEF ID call { $$ = new CDef(ERROR_INFO, $2, $3); }
	| ID assign_op expr { $$ = new VariableAssignement(ERROR_INFO, $1, $3, $2); }
	| expr { $$ = new ExpressionStatement(ERROR_INFO, $1); }
	;

expr:
	literal { $$ = $1; }
	| unary_op { $$ = $1; }
	| call { $$ = $1; }
	| ID { $$ = new VariableUse($1); }
	;

import:
	/* TODO : Imports with . */
	USE ID { $$ = Module($2); }
	;

literal:
	INT { $$ = new Literal($1, "int"); }
	| NUM { $$ = new Literal($1, "num"); }
	| BOOL { $$ = new Literal($1, "bool"); }
	| STR { $$ = new Literal($1, "str"); }
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

call:
	call_start PAR_END { $$ = $1; }
	| call_start expr PAR_END { $$ = $1; $$->args.push_back($2); }
	;

call_start:
	ID PAR_BEGIN { $$ = new Call($1); }
	| call_start expr COMMA { $$ = $1; $$->args.push_back($2); }
	;

new_line:
	NL {}
	| new_line NL {}
	;

%%

void Parser::error(const location &_, const string &msg)
{
	cerr << scanner.file << ":" <<
		scanner.loc.begin.line << ":" <<
		scanner.loc.begin.column << ": " <<
		msg << endl;
}
