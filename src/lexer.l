%{
	#include <iostream>

	#include "scanner.h"
	#include "parser.hpp"

	using namespace std;
	using namespace up;

	#define yyterminate() up::Parser::make_END(loc);

	// Update the location
	#define YY_USER_ACTION UpdateLocation(yyleng);

%}

%option nodefault
%option noyywrap
%option c++
%option yyclass="Scanner"
%option prefix="Up"

letter [a-zA-Z]
number [\-]?[0-9]+

%%

{letter}		return Parser::make_LETTER(yytext, loc);

{number}		{
	// Convert matched string to int
	int number = stoi(yytext);

	return Parser::make_NB(number, loc);
}

\n				loc.lines();

.				{
	cerr << file << ":" << loc.begin.line << ":" << loc.begin.column
		<< ": Invalid token : " << yytext << endl;
}

<<EOF>>		return yyterminate();

%%
