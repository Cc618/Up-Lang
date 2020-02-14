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

/* !!! We use \n for line start and $ for line end : [start, end) */
comment #.*$
/* Empty line */
empty_line \n[ \t]*$
comment_line \n[ \t]*#.*$
/* TODO : Tabs with 4 spaces */
/* Start of line */
tabs \n(\t|[ ]{4})*
/* After the line begining */
space [ \t]

%%

%{
	// Executed for each token
	// Update indents
	if (indentsToAdd > 0)
	{
        // std::cout << "INDENT\n";
		--indentsToAdd;
		// return Parser::make_INDENT(loc);
	}
	else if (indentsToAdd < 0)
	{
        // std::cout << "DEDENT\n";
		++indentsToAdd;
		// return Parser::make_DEDENT(loc);
	}
%}

{comment}		; // Ignored

{empty_line}	loc.lines();
{comment_line}	loc.lines();

{tabs}			{
	// Update lines
	loc.lines();

	updateIndent(yytext, yyleng);
}

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

{id}			return Parser::make_ID(yytext, loc);

{space}			; // Ignored

.				{
	cerr << file << ":" << loc.begin.line << ":" << loc.begin.column
		<< ": Invalid token : " << yytext << endl;
}

<<EOF>>		return yyterminate();

%%
