%{

#include <stdlib.h>
#include <stdio.h>
#include "parser.tab.h"



        
%}

%%
"print\""(.)*"\"" {yylval.id  = strdup(yytext+6); return T_PRINT;}
"Cercle" return T_CIRCLE;
"Draw"	return T_DRAW;
"Bezier" return T_BEZIER;
"--" return T_PATH;
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
"pop" return T_POP;
"tail" return T_NEXT;
"next" return T_NEXT;
"hd" return T_POP;
"tl" return T_NEXT;
"::" return T_PUSH;
"translate" return T_TRANS;
"rotation" return T_ROT;
"homotetie" return T_HOM;

[(),\[\]{}]         {return yytext[0];}
[[:space:]] ;
[0-9]+ {yylval.num = atoi(yytext); return T_NUM;}
[[:alpha:]_]+    {yylval.id = strdup (yytext); return T_ID;}


.               {ECHO; yyerror ("unexpected character");}
