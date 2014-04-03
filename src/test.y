%{

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "../interface/machine.h"

  struct env *env = NULL;
  struct configuration concrete_conf;
  struct configuration *conf = &concrete_conf;
  struct closure *temp;
%}



%union {
  char* id;
  int num;
  struct expr *expr;
  struct env *env;
}

%token<num>T_NUM
%token<id>T_ID
%token FIN_EXPR T_PLUS T_MINUS T_MULT T_DIV T_LEQ T_LE T_GEQ T_GE T_EQ T_OR T_AND T_NOT T_EQUAL T_IF T_ELSE T_THEN T_FUN T_ARROW T_LET T_IN T_WHERE

%right T_EQUAL T_ARROW
%right T_LET T_FUN T_IF T_THEN 
%right T_ELSE
%left T_PLUS T_MINUS
%left T_MULT T_DIV
%right  T_LEQ T_LE T_GE T_GEQ T_OR T_AND T_NOT T_EQ

%type<expr> e arg_list
%type<env> en
	
%%
s       :s e[expr] FIN_EXPR {conf->closure = mk_closure($expr,env); conf->stack=NULL; step(conf); 
   if(conf->closure->expr->type==NUM){
     printf(">>> %d\n",conf->closure->expr->expr->num);
   }	
 }
|s en FIN_EXPR {env = $2;print_env(env);}
|
;
                                      
en  :T_LET T_ID[x] T_EQUAL e[expr]                                  {$$ = push_rec_env($x,$expr,env);}
    ;
	
e   :T_NUM                                                          {$$ = mk_int($1);}        
	|e T_PLUS e                                                     {$$ = mk_app(mk_app(mk_op(PLUS),$1),$3);}
	|e T_MINUS e                                                    {$$ = mk_app(mk_app(mk_op(MINUS),$1),$3);}
	|e T_DIV e                                                      {$$ = mk_app(mk_app(mk_op(DIV),$1),$3);}
	|e T_MULT e                                                     {$$ = mk_app(mk_app(mk_op(MULT),$1),$3);}
	|e T_LEQ e                                                      {$$ = mk_app(mk_app(mk_op(LEQ),$1),$3) ;}
	|e T_LE e                                                       {$$ = mk_app(mk_app(mk_op(LE),$1),$3) ;}
	|e T_GEQ e                                                      {$$ = mk_app(mk_app(mk_op(GEQ),$1),$3) ;}
	|e T_GE e                                                       {$$ = mk_app(mk_app(mk_op(GE),$1),$3) ;}
	|e T_OR e                                                       {$$ = mk_app(mk_app(mk_op(OR),$1),$3) ;}
	|e T_AND e                                                      {$$ = mk_app(mk_app(mk_op(AND),$1),$3) ;}
	|T_ID                                                           {$$ = mk_id($1);}
	|e T_EQ e                                                       {$$ = mk_app(mk_app(mk_op(EQ),$1),$3) ;}
	|T_NOT e[expr]                                                  {$$ = mk_app(mk_op(NOT),$expr) ;}
    |T_FUN T_ID[var] arg_list[expr]                                 {$$ = mk_fun($var,$expr);env = push_rec_env($var,$$,env);}                    
	|T_LET T_ID[x] T_EQUAL e[arg] T_IN e[exp]		            	{$$ = mk_app(mk_fun($x,$exp),$arg) env = push_rec_env($x,$$,env);}
	|e[exp] T_WHERE T_ID[x] T_EQUAL e[arg]			            	{$$ = mk_app(mk_fun($x,$exp),$arg) env = push_rec_env($x,$$,env);}
	|T_IF e[cond] T_THEN e[then_br] T_ELSE e[else_br]               {$$ = mk_cond($cond, $then_br, $else_br) ;}
    |e[fun] e[arg]                                                  {$$ = mk_app($fun,$arg);}
    |'(' e ')'                                                      {$$ = $2;}
    ;


arg_list:T_ARROW e                                                  {$$=$2;}
        |T_ID[var] arg_list                                         {$$=mk_fun ($1, $2); env = push_rec_env($var,$$,env);}
        ;

/*apli       :apli[fun] e[arg]                                           {$$ = mk_app($fun,$arg);}
           |e                                                          {$$ = $1;}
           ;
*/

%%

int main(int argc, char *argv[])
{
      yyparse();

  return EXIT_SUCCESS;
}
