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
	{
		auto tok = scanner.nextToken();

		// TODO : Remove debug
		// cout << "@" << scanner.loc.begin.line << " : ";
		// if (tok.token() == Parser::token::TOKEN_NL)
		// 	cout << "NL\n";
		// else if (tok.token() == Parser::token::TOKEN_DEDENT)
		// 	cout << "DEDENT\n";
		// else if (tok.token() == Parser::token::TOKEN_INDENT)
		// 	cout << "INDENT\n";
		// else
		// 	cout << tok.token() << endl;

		return tok;
	}

	// Used for statements
	#define ERROR_INFO scanner.errorInfo()
}

%token
	END 0					"End of file"
	PASS					"Pass token"
	START					"Start of file"
	NL						"New line"
	TERMINATE				"Terminate (;)"
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
	<int> INDENT_UPDT		"Indentation update"
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
%type <Module>				import;
%type <Call*>				call;
%type <Call*>				call_start;
%type <Block*>				block;
%type <Block*>				block_start;
%type <string>				assign_op;
%type <char>				new_line;


%start program

%nonassoc INC DEC USE CDEF

%left START
%left END
%left NL

%%
program:
	| program block { cout << "BLOCK" << $2->statements.size() << endl; /* << $2->toString(); */ }
	| program stmt { compiler.main()->statements.push_back($2); }
	| program import { compiler.import($2); }
	| program START new_line {}
	| program START {}
	;

block:
	block_start DEDENT { $$ = $1; }
	| block new_line { $$ = $1; /* New line after unindentation is ignored */ }
	;

block_start:
	INDENT stmt { $$ = new Block(); $$->statements.push_back($2); }
	| block_start stmt { $$ = $1; $$->statements.push_back($2); }
	;


stmt:
	ID ID EQ expr new_line 			{ $$ = new VariableDeclaration(ERROR_INFO, $2, $1, $4); }
	| AUTO ID EQ expr new_line 		{ $$ = new VariableDeclaration(ERROR_INFO, $2, "auto", $4); }
	| ID ID new_line				{ $$ = new VariableDeclaration(ERROR_INFO, $2, $1, nullptr); }
	| CDEF ID call new_line 		{ $$ = new CDef(ERROR_INFO, $2, $3); }
	| ID assign_op expr new_line 	{ $$ = new VariableAssignement(ERROR_INFO, $1, $3, $2); }
	| expr new_line 				{ $$ = new ExpressionStatement(ERROR_INFO, $1); }
	;

expr:
	literal { $$ = $1; }
	| unary_op { $$ = $1; }
	| call { $$ = $1; }
	| ID { $$ = new VariableUse($1); }
	;

import:
	/* TODO : Imports with . */
	USE ID new_line { $$ = Module($2); }
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
	| TERMINATE {}
	| new_line NL {}
	| END {}
	;

%%

void Parser::error(const location &_, const string &msg)
{
	cerr << scanner.file << ":" <<
		scanner.loc.begin.line << ":" <<
		scanner.loc.begin.column << ": " <<
		msg << endl;
}
