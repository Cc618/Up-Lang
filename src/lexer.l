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

/* TODO : Float */
number [\-]?[0-9]+
id [a-zA-Z][a-zA-Z0-9]*

/* TODO : Comments */
/* Empty line */
empty_line \n[ \t]*$
/* TODO : Tabs with 4 spaces */
/* Start of line */
tabs \n[\t]*
/* After the linme begining */
space [ \t]


%%

{empty_line}	loc.lines();

{tabs}			{
	// Update lines
	loc.lines();

	updateIndent(yytext, yyleng);
}

{id}			return Parser::make_ID(yytext, loc);

{number}		{
	// Convert matched string to int
	int number = stoi(yytext);

	return Parser::make_NB(number, loc);
}

"="				return Parser::make_EQ(loc);

\n				loc.lines();

{space}			; // Ignored

.				{
	cerr << file << ":" << loc.begin.line << ":" << loc.begin.column
		<< ": Invalid token : " << yytext << endl;
}

<<EOF>>		return yyterminate();

%%
