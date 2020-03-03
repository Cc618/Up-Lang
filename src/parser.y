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
	#include "colors.h"

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
		// else if (tok.token() == Parser::token::TOKEN_END)
		// 	cout << "END\n";
		// else
		// 	cout << tok.token() << endl;

		return tok;
	}

	// Error but use another location
	#define LOC_ERROR(LOC) ErrorInfo(scanner.module, LOC.begin.line, LOC.begin.column)

	// Shortcut for errors
	// !!! TODO : DEPRECATED
	#define ERROR_INFO scanner.errorInfo()
}

%token
	END 0					"End of file"
	DOUBLE_END				"End of file (double state)"
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
	MODEQ					"%="
	INC						"++"
	DEC						"--"
	IS						"=="
	LEQ						"<="
	LESS					"<"
	AEQ						">="
	ABOV					">"
	ADD						"+"
	SUB						"-"
	MUL						"*"
	DIV						"/"
	MOD						"%"
	PAR_BEGIN				"("
	PAR_END					")"
	COMMA					","
	ELLIPSIS				"..."
	AUTO					"$"
	IF						"?"
	OR						"or keyword"
	WHILE					"while keyword"
	FOR						"for keyword"
	TO						"to keyword"
	USE						"use keyword"
	CDEF					"cdef keyword"
	RET						"ret keyword"
	<int> INDENT_UPDT		"Indentation update"
	<string> ID				"Identifier"
	<string> INT			"Integer (int)"
	<string> NUM			"Float number (num)"
	<string> BOOL			"Boolean (bool)"
	<string> STR			"String (str)"
	<string> CCODE			"C Code section"
;

%type <Statement*>				stmt;
%type <ConditionSequence*>		conditions;
%type <ControlStatement*>		or_if_stmt;
%type <ControlStatement*>		if_stmt;
%type <OrStatement*>			or_stmt;
%type <Expression*>				expr;
%type <Function*>				function;
%type <Literal*>				literal;
%type <UnaryOperation*>			unary_op;
%type <Module>					import;
%type <Call*>					call;
%type <Call*>					call_start;
%type <Block*>					block;
%type <Block*>					block_start;
%type <string>					assign_op;
%type <char>					new_line;
%type <std::vector<Argument*>>	args;
%type <std::vector<Argument*>>	args_start;

%start program

%nonassoc INC DEC USE CDEF
%left START
%left END
%left NL
%left FOR WHILE IF
%left OR
%left IS LESS LEQ AEQ ABOV
%left ADD SUB
%left MUL DIV MOD

%%
program:
	| program DOUBLE_END			{ YYACCEPT; /* Avoid infinite ends bug */ }
	| program START new_line		{}
	| program START					{}
	| program stmt					{ compiler.pushGlobalStatement($2); }
	| program function				{ compiler.addFunction($2); }
	| program import				{ compiler.import($2, LOC_ERROR(@2)); }
	;

function:
	ID ID args new_line block		{ $$ = new UpFunction(ERROR_INFO, $1, $2, $3, $5); }
	| CDEF ID ID args new_line 		{ $$ = new Function(ERROR_INFO, $2, $3, $4); }
	;

block:
	block_start DEDENT				{ $$ = $1; }
	| block new_line				{ $$ = $1; /* New line after unindentation is ignored */ }
	;

block_start:
	INDENT stmt						{ $$ = new Block(LOC_ERROR(@2)); $$->content.push_back($2); }
	| block_start stmt				{ $$ = $1; $$->content.push_back($2); }
	;

args:
	PAR_BEGIN PAR_END				{ $$ = {}; }
	| PAR_BEGIN ELLIPSIS PAR_END	{ $$ = { Argument::createEllipsis(ERROR_INFO) }; }
	| args_start PAR_END			{ $$ = $1; }
	;

args_start:
	PAR_BEGIN ID ID					{ $$ = { new Argument(ERROR_INFO, $2, $3) }; }
	| args_start COMMA ID ID		{ $$ = $1; $$.push_back(new Argument(ERROR_INFO, $3, $4)); }
	;

stmt:
	ID ID EQ expr new_line 			{ $$ = new VariableDeclaration(ERROR_INFO, $2, $1, $4); }
	| AUTO ID EQ expr new_line 		{ $$ = new VariableDeclaration(ERROR_INFO, $2, "auto", $4); }
	| AUTO ID ID new_line			{ $$ = new VariableDeclaration(ERROR_INFO, $3, $2, nullptr); }
	| ID assign_op expr new_line 	{ $$ = new VariableAssignement(ERROR_INFO, $1, $3, $2); }
	| RET expr new_line 			{ $$ = new Return(ERROR_INFO, $2); }
	| RET new_line	 				{ $$ = new Return(ERROR_INFO, nullptr); }
	| WHILE expr new_line block		{ $$ = new ControlStatement(ERROR_INFO, $2, $4, "while"); }
	| FOR ID EQ expr TO
		expr new_line block			{ $$ = new ForStatement(ERROR_INFO, $2, $4, $6, $8); }
	| FOR ID TO expr new_line block	{ $$ = ForStatement::createDefaultInit(ERROR_INFO, $2, $4, $6); }
	| expr new_line 				{ $$ = new ExpressionStatement(ERROR_INFO, $1); }
	| conditions					{ $$ = $1; }
	| CCODE new_line				{ $$ = new CStatement(ERROR_INFO, $1.substr(2, $1.size() - 4)); }
	;

conditions:
	if_stmt							{ $$ = new ConditionSequence(LOC_ERROR(@1), $1); }
	| conditions or_if_stmt			{ $$ = $1; $$->controls.push_back($2); }
	| conditions or_stmt			{ $$ = $1; $$->controls.push_back($2); }
	;

or_if_stmt:
	OR expr IF new_line block		{ $$ = new ControlStatement(LOC_ERROR(@1), $2, $5, "else if"); }
	;

if_stmt:
	expr IF new_line block			{ $$ = new ControlStatement(LOC_ERROR(@1), $1, $4, "if"); }
	;

or_stmt:
	OR new_line block				{ $$ = new OrStatement(LOC_ERROR(@1), $3); }
	;

expr:
	PAR_BEGIN expr PAR_END			{ $$ = $2; }
	| literal						{ $$ = $1; }
	| unary_op						{ $$ = $1; }
	| call							{ $$ = $1; }
	| expr ADD expr					{ $$ = new BinaryOperation(LOC_ERROR(@2), $1, $3, "+"); }
	| expr SUB expr					{ $$ = new BinaryOperation(LOC_ERROR(@2), $1, $3, "-"); }
	| expr MUL expr					{ $$ = new BinaryOperation(LOC_ERROR(@2), $1, $3, "*"); }
	| expr DIV expr					{ $$ = new BinaryOperation(LOC_ERROR(@2), $1, $3, "/"); }
	| expr MOD expr					{ $$ = new BinaryOperation(LOC_ERROR(@2), $1, $3, "%"); }
	| expr IS expr					{ $$ = new BinaryOperation(LOC_ERROR(@2), $1, $3, "==", true); }
	| expr LEQ expr					{ $$ = new BinaryOperation(LOC_ERROR(@2), $1, $3, "<=", true); }
	| expr LESS expr				{ $$ = new BinaryOperation(LOC_ERROR(@2), $1, $3, "<", true); }
	| expr AEQ expr					{ $$ = new BinaryOperation(LOC_ERROR(@2), $1, $3, ">=", true); }
	| expr ABOV expr				{ $$ = new BinaryOperation(LOC_ERROR(@2), $1, $3, ">", true); }
	| ID							{ $$ = new VariableUsage(ERROR_INFO, $1); }
	;

import:
	/* TODO : Imports with . */
	USE ID new_line					{ $$ = Module($2, true, scanner.module.folder); }
	;

literal:
	INT								{ $$ = new Literal(ERROR_INFO, $1, "int"); }
	| NUM							{ $$ = new Literal(ERROR_INFO, $1, "num"); }
	| BOOL							{ $$ = new Literal(ERROR_INFO, $1, "bool"); }
	| STR							{ $$ = new Literal(ERROR_INFO, $1, "str"); }
	;

assign_op:
	EQ								{ $$ = "="; }
	| ADDEQ							{ $$ = "+="; }
	| SUBEQ							{ $$ = "-="; }
	| MULEQ							{ $$ = "*="; }
	| DIVEQ							{ $$ = "/="; }
	| MODEQ							{ $$ = "%="; }
	;

unary_op:
	ID INC							{ $$ = new UnaryOperation(LOC_ERROR(@2), $1, "++"); }
	| ID DEC						{ $$ = new UnaryOperation(LOC_ERROR(@2), $1, "--"); }
	| INC ID						{ $$ = new UnaryOperation(LOC_ERROR(@1), $2, "++", true); }
	| DEC ID						{ $$ = new UnaryOperation(LOC_ERROR(@1), $2, "--", true); }
	;

call:
	call_start PAR_END				{ $$ = $1; }
	| call_start expr PAR_END		{ $$ = $1; $$->args.push_back($2); }
	;

call_start:
	ID PAR_BEGIN					{ $$ = new Call(ErrorInfo(scanner.module, @1.begin.line, @1.begin.column), $1); }
	| call_start expr COMMA			{ $$ = $1; $$->args.push_back($2); }
	;

new_line:
	NL								{}
	| TERMINATE						{}
	| new_line NL					{}
	| END							{}
	;

%%

void Parser::error(const location &_, const string &msg)
{
	cerr << "File " << YELLOW << scanner.module.path() << DEFAULT <<
        ":" << BLUE << scanner.loc.begin.line << DEFAULT << ":" << BLUE <<
		scanner.loc.begin.column << DEFAULT << ": " <<
		msg << endl;
}
