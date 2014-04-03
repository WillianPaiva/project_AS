%{

#include <stdlib.h>
#include <stdio.h>
#include "test.tab.h"

        
%}

%%

"print\""(.)*"\"" {yylval.id = strdup (yytext+5); return T_PRINT; }
"->" return T_ARROW;
"+" return T_PLUS;
"-" return T_MINUS;
"/" return T_DIV;
"*" return T_MULT;
"==" return T_EQ;
";" return FIN_EXPR;
"<=" return T_LEQ;
">=" return T_GEQ;
"=" return T_EQUAL;
">" return T_GE;
"<" return T_LE;
"||" return T_OR;
"&&" return T_AND;
"!" return T_NOT;
"if" return T_IF;
"then" return T_THEN;
"else" return T_ELSE;
"fun" return T_FUN;
"let" return T_LET;
"in" return T_IN;
"where" return T_WHERE;
"list" return T_LIST;

[[:space:]] ;
[[:digit:]]+ {yylval.num = atoi(yytext); return T_NUM;}
[a-zA-z]+    {yylval.id = strdup (yytext); return T_ID;}
[(),]            return yytext[0];
.               {ECHO; yyerror ("unexpected character");}
