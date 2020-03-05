%{
	#include <iostream>
	#include <string>

	#include "scanner.h"
	#include "compiler.h"
	#include "error_info.h"
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
if \?

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
/* C section */
ccode %\{([^%]|%[^\}])*%\}
/* ccode \%\{^(\%\})*\%\} */


%%

{ccode}			return Parser::make_CCODE(yytext, loc);

{comment}		; // Ignored
{empty_line}	; // Ignored
{comment_line}	; // Ignored

{tabs}			return Parser::make_INDENT_UPDT(countTabs(yytext, yyleng), loc);

{str}			return Parser::make_STR(yytext, loc);
{int}			return Parser::make_INT(yytext, loc);
{num}			return Parser::make_NUM(yytext, loc);
{bool}			return Parser::make_BOOL(yytext, loc);

{auto}			return Parser::make_AUTO(loc);
{if}			return Parser::make_IF(loc);

"="				return Parser::make_EQ(loc);
"+="			return Parser::make_ADDEQ(loc);
"-="			return Parser::make_SUBEQ(loc);
"*="			return Parser::make_MULEQ(loc);
"/="			return Parser::make_DIVEQ(loc);
"%="			return Parser::make_MODEQ(loc);
"++"			return Parser::make_INC(loc);
"--"			return Parser::make_DEC(loc);
"=="			return Parser::make_IS(loc);
"<="			return Parser::make_LEQ(loc);
"<"				return Parser::make_LESS(loc);
">="			return Parser::make_AEQ(loc);
">"				return Parser::make_ABOV(loc);
"+"				return Parser::make_ADD(loc);
"-"				return Parser::make_SUB(loc);
"*"				return Parser::make_MUL(loc);
"/"				return Parser::make_DIV(loc);
"%"				return Parser::make_MOD(loc);

"."				return Parser::make_PERIOD(loc);
"("				return Parser::make_PAR_BEGIN(loc);
")"				return Parser::make_PAR_END(loc);
","				return Parser::make_COMMA(loc);
"..."			return Parser::make_ELLIPSIS(loc);

"or"			return Parser::make_OR(loc);
"while"			return Parser::make_WHILE(loc);
"for"			return Parser::make_FOR(loc);
"to"			return Parser::make_TO(loc);
"use"			return Parser::make_USE(loc);
"cdef"			return Parser::make_CDEF(loc);
"ret"			return Parser::make_RET(loc);

{id}			return Parser::make_ID(yytext, loc);

"\n"			{ loc.lines(); return Parser::make_NL(loc); }	
";"				return Parser::make_TERMINATE(loc);

{space}			; // Ignored

.				{
	compiler.generateError(std::string("Invalid token : ") + yytext, ErrorInfo(module, loc.begin.line, loc.begin.column), true);
}

<<EOF>>			return yyterminate();

%%
