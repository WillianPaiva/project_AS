%{

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "../interface/machine.h"

  struct env * env=NULL;
  struct configuration concrete_conf;
  struct configuration * conf = & concrete_conf;
%}



%union {
  char* id;
  int num;
  struct expr * expr;
}

%token<num>T_NUM
%token<id>T_ID
%token FIN_EXPR T_PLUS T_MINUS T_MULT T_DIV T_LEQ T_LE T_GEQ T_GE T_EQ T_OR T_AND T_NOT T_EQUAL T_IF T_ELSE T_THEN T_FUN T_ARROW T_LET

%right T_EQUAL T_ARROW
%right T_LET T_FUN T_IF T_THEN 
%right T_ELSE
%left T_PLUS T_MINUS
%left T_MULT T_DIV
%right  T_LEQ T_LE T_GE T_GEQ T_OR T_AND T_NOT T_EQ

%type<expr> e
	
%%
s: 
	s e[expr] FIN_EXPR {conf->closure = mk_closure($expr,env); conf->stack=NULL; step(conf); 
		if(conf->closure->expr->type==NUM){
		printf(">>> %d\n",conf->closure->expr->expr->num);
		}	
	}
        | 	T_LET T_ID[x] T_EQUAL e[expr] FIN_EXPR {env = push_rec_env($x,$expr,env);}
        |
	;


	
e:              T_NUM     {$$ = mk_int($1);}        
	|       e T_PLUS e  {$$ = mk_app(mk_app(mk_op(PLUS),$1),$3);}
        |       e T_MINUS e {$$ = mk_app(mk_app(mk_op(MINUS),$1),$3);}
        |       e T_DIV e   {$$ = mk_app(mk_app(mk_op(DIV),$1),$3);}
        |       e T_MULT e   {$$ = mk_app(mk_app(mk_op(MULT),$1),$3);}
	|	e T_LEQ e {$$ = mk_app(mk_app(mk_op(LEQ),$1),$3) ;}
	|	e T_LE e {$$ = mk_app(mk_app(mk_op(LE),$1),$3) ;}
	|	e T_GEQ e {$$ = mk_app(mk_app(mk_op(GEQ),$1),$3) ;}
	|	e T_GE e {$$ = mk_app(mk_app(mk_op(GE),$1),$3) ;}
	|	e T_OR e {$$ = mk_app(mk_app(mk_op(OR),$1),$3) ;}
	|	e T_AND e {$$ = mk_app(mk_app(mk_op(AND),$1),$3) ;}
	|	"(" T_NOT e[expr] ")" {$$ = mk_app(mk_op(NOT),$expr) ;}
	|	T_ID {$$ = mk_id($1);}
	|	"(" T_IF e[cond] T_THEN e[then_br] T_ELSE e[else_br] ")" {$$ = mk_cond($cond, $then_br, $else_br) ;}
	|	"(" T_FUN T_ID[var] T_ARROW e[expr] ")" {$$ = mk_fun($var,$expr);}
        |       "(" e[fun] e[arg] ")" {$$=mk_app($fun,$arg);}     
	|	"(" e ")" {$$ = $2;}
        ;

%%

int main(int argc, char *argv[])
{
  
  yyparse();

  return EXIT_SUCCESS;
}
