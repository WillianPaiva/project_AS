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
  void print_exp(struct configuration *conf){ 
	if(conf->closure->expr->type==NUM){
     printf(">>> %d\n",conf->closure->expr->expr->num);
   }
    if(conf->closure->expr->type==POINT){
     printf(">>> POINT X=%d Y=%d \n", conf->closure->expr->expr->point.x->expr->num, conf->closure->expr->expr->point.y->expr->num);
   }
   }

%}



%union {
  char* id;
  int num;
  struct expr *expr;
  struct env *env;
}

/*Tokens utilisés*/
%token<num>T_NUM T_NNUM
%token<id>T_ID T_PRINT
%token FIN_EXPR T_PLUS T_MINUS T_MULT T_DIV T_LEQ T_LE T_GEQ T_GE T_EQ T_OR T_AND T_NOT T_EQUAL T_IF T_ELSE T_THEN T_FUN T_ARROW T_LET T_IN T_WHERE T_NEXT T_POP T_PUSH T_PATH T_CIRCLE T_DRAW T_BEZIER 


 /*Priorités nécessaires*/
%nonassoc T_EQUAL T_ARROW T_LET T_FUN T_IF T_THEN T_WHERE T_IN T_ELSE
%left  T_LEQ T_LE T_GE T_GEQ T_EQ
%left T_OR 
%left T_AND T_PATH
%nonassoc T_NOT T_POP T_NEXT
%left T_PLUS T_MINUS
%left T_MULT T_DIV T_PUSH 
%left FUNCTION_APPLICATION T_NUM T_NNUM T_ID '{' '('

/* Déclaration des types*/
%type<expr> e arg_list list 
%type<env> en
	
%%

 /*GRAMMAIRE UTILISEE*/

 /*Terminal*/
s       :s e[expr] FIN_EXPR {conf->closure = mk_closure($expr,env); conf->stack=NULL; step(conf); print_exp(conf);
    
 }
|s en FIN_EXPR				   {env = $2;}
|s T_PRINT FIN_EXPR			   {$2[strlen($2)-1] = 0;printf("%s\n",$2);}
|s T_DRAW '(' e[d] ')' FIN_EXPR   {conf->closure = mk_closure(mk_app(mk_op(DRAW),$d),env); conf->stack=NULL; step(conf);}	
|
;

/*Variable d'environnement lors de l'affectation de variables*/
en  :T_LET T_ID[x] T_EQUAL e[expr]                                  {$$ = push_rec_env($x,$expr,env);}
    ;

/*Expressions régulières*/


/*Reconnaissance d'entiers*/
e   : T_NUM												   { $$ = mk_int($1);}
	| T_NNUM											   { $$ = mk_int(-$1);}
	| T_POP e[l]										   { $$ = mk_app(mk_op(POP),$l);}
	| T_NEXT e[l]										   { $$ = mk_app(mk_op(NEXT),$l);}
	|'{' e[x] ',' e[y] '}'								   { $$ = mk_point($x,$y);}	
	| T_BEZIER '(' e[p1] ',' e[p2] ',' e[p3] ',' e[p4] ')' { $$ = mk_bezier($p1,mk_bezier($p2,mk_bezier($p3,mk_bezier($p4,NULL))));}
	| T_CIRCLE '(' e[c] ',' e[r] ')'                       { $$ = mk_circle($c,$r);}
	| e T_PATH e                                           { $$ = mk_path($1,mk_path($3,NULL));}
	| e T_PLUS e                                           { $$ = mk_app(mk_app(mk_op(PLUS),$1),$3);}
	| e T_MINUS e                                          { $$ = mk_app(mk_app(mk_op(MINUS),$1),$3);}
	| e T_DIV e                                            { $$ = mk_app(mk_app(mk_op(DIV),$1),$3);}
	| e T_MULT e                                           { $$ = mk_app(mk_app(mk_op(MULT),$1),$3);}
	| e T_LEQ e                                            { $$ = mk_app(mk_app(mk_op(LEQ),$1),$3) ;}
	| e T_LE e                                             { $$ = mk_app(mk_app(mk_op(LE),$1),$3) ;}
	| e T_GEQ e                                            { $$ = mk_app(mk_app(mk_op(GEQ),$1),$3) ;}
	| e T_GE e                                             { $$ = mk_app(mk_app(mk_op(GE),$1),$3) ;}
	| e T_OR e                                             { $$ = mk_app(mk_app(mk_op(OR),$1),$3) ;}
	| e T_AND e                                            { $$ = mk_app(mk_app(mk_op(AND),$1),$3) ;}
	| T_ID                                                 { $$ = mk_id($1);}/*Reconnaissance d'identificateurs et de variables*/
	| e T_EQ e                                             { $$ = mk_app(mk_app(mk_op(EQ),$1),$3) ;}
	| T_NOT e[expr]                                        { $$ = mk_app(mk_op(NOT),$expr) ;}
	| T_FUN T_ID[var] arg_list[expr]                       { $$ = mk_fun($var,$expr);} /*Définition de fonctions*/
	| T_LET T_ID[x] T_EQUAL e[arg] T_IN e[exp]             { $$ = mk_app(mk_fun($x,$exp),$arg);}/*Fonction IN*/
	| e[exp] T_WHERE T_ID[x] T_EQUAL e[arg]                { $$ = mk_app(mk_fun($x,$exp),$arg); }/*Fonction WHERE*/
	| T_IF e[cond] T_THEN e[then_br] T_ELSE e[else_br]     { $$ = mk_cond($cond, $then_br, $else_br) ;}
	| '[' list[l] ']'                                      { $$ = $l;}/*OP sur Listes*/
	| e[exp] T_PUSH e[l]                                   { $$ = mk_app(mk_app(mk_op(PUSH),$exp),$l);}
	| e[fun] e[arg] %prec FUNCTION_APPLICATION             { $$ = mk_app($fun,$arg);}/*Exécution de fonctions à plusieurs variables*/
	| '(' e ')'                                            { $$ = $2;}/*Ignorer les parentheses inutiles*/
    ;
/*Boucle pour plusieurs paramtres d'une fonction*/
		



/*Définition multiple de fonctions*/
arg_list:T_ARROW e                                                  {$$=$2;}
        |T_ID[var] arg_list                                         {$$=mk_fun ($1, $2);}
        ;

list	: e[ex]					{$$ = mk_cell($ex,mk_nil());}
		| e[ex] ',' list[l]		{$$ = mk_cell($ex,$l);}
		|	     				{$$ = mk_nil();}
		;

%%

int main(int argc, char *argv[])
{
      yyparse();

  return EXIT_SUCCESS;
}
