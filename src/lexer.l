%{
	#include <iostream>

	#include "scanner.h"
	#include "parser.hpp"

	using namespace std;
	using namespace up;

	#define yyterminate() up::Parser::make_END(loc);

	// Update the location
	#define YY_USER_ACTION updateLocation(yyleng);

%}

%option nodefault
%option noyywrap
%option c++
%option yyclass="Scanner"
%option prefix="Up"

id [a-zA-Z][a-zA-Z0-9]*
auto \$

int [\-]?[0-9]+
num [\-]?[0-9]+\.[0-9]*
bool yes|no
str '(.|\\')*'

/* !!! We use ^ for line start and $ for line end : (start, end) */
comment #.*$
/* Empty line */
empty_line ^[ \t]+$
comment_line ^[ \t]*#.*$
/* Start of line */
tabs ^(\t|[ ]{4})*
/* After the line begining */
space [ \t]


%%

{comment}		; // Ignored
{empty_line}	; // Ignored
{comment_line}	; // Ignored

{tabs}			return Parser::make_INDENT_UPDT(countTabs(yytext, yyleng), loc);

{str}			return Parser::make_STR(yytext, loc);
{int}			return Parser::make_INT(yytext, loc);
{num}			return Parser::make_NUM(yytext, loc);
{bool}			return Parser::make_BOOL(yytext, loc);

{auto}			return Parser::make_AUTO(loc);

"="				return Parser::make_EQ(loc);
"+="			return Parser::make_ADDEQ(loc);
"-="			return Parser::make_SUBEQ(loc);
"*="			return Parser::make_MULEQ(loc);
"/="			return Parser::make_DIVEQ(loc);
"++"			return Parser::make_INC(loc);
"--"			return Parser::make_DEC(loc);

"("				return Parser::make_PAR_BEGIN(loc);
")"				return Parser::make_PAR_END(loc);
","				return Parser::make_COMMA(loc);
"..."			return Parser::make_ELLIPSIS(loc);

"use"			return Parser::make_USE(loc);
"cdef"			return Parser::make_CDEF(loc);

{id}			return Parser::make_ID(yytext, loc);

"\n"			{ loc.lines(); return Parser::make_NL(loc); }	
";"				return Parser::make_TERMINATE(loc);

{space}			; // Ignored

.				{
	cerr << file << ":" << loc.begin.line << ":" << loc.begin.column
		<< ": Invalid token : " << yytext << endl;
}

<<EOF>>		return yyterminate();

%%
