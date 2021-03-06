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

  /*******************************************/
  /* FONCTION POUR L'AFFICHAGE DANS LE SHELL */
  /*******************************************/

  void print_exp(struct configuration *conf){ 
    if(conf->closure->expr->type==NUM){
      printf(">>> %d\n",conf->closure->expr->expr->num);
    }
    if(conf->closure->expr->type==POINT){

      struct expr * px = conf->closure->expr->expr->point.x;
      while(px->type == ID){
	px = id(px,conf->closure->env);
			
      }

      struct expr * py = conf->closure->expr->expr->point.y;
      while(py->type == ID){
	py = id(py,conf->closure->env);
			
      }

      printf(">>> POINT X=%d Y=%d \n",px->expr->num , py->expr->num );
    }
  }

  
  
  struct expr * step_conf(struct expr *ex){
    conf->closure = mk_closure(ex,env);
    conf->stack=NULL;
    step(conf);
    return conf->closure->expr;
   
  }

  %}

/***********/
/** UNION **/
/***********/
%union {
  char* id;
  int num;
  struct expr *expr;
  struct env *env;
}

/*Tokens utilisés*/
%token<num>T_NUM 
%token<id>T_ID T_PRINT
%token FIN_EXPR T_PLUS T_MINUS T_MULT T_DIV T_LEQ T_LE T_GEQ T_GE T_EQ T_OR T_AND T_NOT T_EQUAL T_IF T_ELSE T_THEN T_FUN T_ARROW T_LET T_IN T_WHERE T_NEXT T_POP T_PUSH T_PATH T_CIRCLE T_DRAW T_BEZIER T_TRANS T_ROT T_HOM


 /*Priorités nécessaires*/
%nonassoc T_EQUAL T_ARROW T_LET T_FUN T_IF T_THEN T_WHERE T_IN T_ELSE 
%left  T_LEQ T_LE T_GE T_GEQ T_EQ T_PUSH T_TRANS T_ROT T_HOM
%left T_OR 
%left T_AND T_PATH T_BEZIER T_CIRCLE 
%nonassoc T_NOT T_POP T_NEXT 
%left T_PLUS T_MINUS
%left T_MULT T_DIV 
%left FUNCTION_APPLICATION T_NUM T_ID '{' '(' '[' 

/* Déclaration des types*/
%type<expr> e arg_list  list 
%type<env> en




	
%%

 /*GRAMMAIRE UTILISEE*/

 /*Terminal*/
s       :s e[expr] FIN_EXPR                        {step_conf($expr); print_exp(conf);}
	|s en FIN_EXPR				   {env = $2;}
	|s T_PRINT FIN_EXPR			   {$2[strlen($2)-1] = 0;printf("%s\n",$2);}
	|
	;

/*Variable d'environnement lors de l'affectation de variables*/
en  :	T_LET T_ID[x] T_EQUAL e[expr]              {$$ = push_rec_env($x,$expr,env);}
	;




/*Reconnaissance d'entiers*/
e   : e T_MINUS e                                              { $$ = mk_app(mk_app(mk_op(MINUS),$1),$3);}
	| T_NUM						       { $$ = mk_int($1);}
	| T_MINUS e					       { $$ = mk_app(mk_app(mk_op(MULT),mk_int(-1)),$2);}
	/* fonction draw */
	| T_DRAW '(' e[d] ')'				       { $$ = mk_app(mk_op(DRAW),$d); }	

	/* figures + opérations sur figures */
	|'{' e[x] ',' e[y] '}'				       { $$ = mk_point($x,$y);}	
	| T_BEZIER '(' e[p1] ',' e[p2] ',' e[p3] ',' e[p4] ')' { $$ = mk_bezier($p1,mk_bezier($p2,mk_bezier($p3,mk_bezier($p4,NULL))));}
	| T_CIRCLE '(' e[c] ',' e[r] ')'                       { $$ = mk_circle($c,$r);}
	| T_TRANS '(' e[fig] ',' e[vect] ')'		       { $$ = step_conf(mk_app(mk_app(mk_op(TRANS),$fig),$vect));}
	| T_ROT '(' e[fig] ',' e[centre] ',' e[rap] ')'	       { $$ = step_conf(mk_app(mk_app(mk_app(mk_op(ROT),$fig),$centre),$rap));}
	| T_HOM '(' e[fig] ',' e[centre] ',' e[rap] ')'	       { $$ = step_conf(mk_app(mk_app(mk_app(mk_op(HOM),$fig),$centre),$rap));}
	| e T_PATH e                                           { $$ = mk_path($1,mk_path($3,NULL));}

	/* op de bases */
	| e T_PLUS e                                           { $$ = mk_app(mk_app(mk_op(PLUS),$1),$3);}
	| e T_DIV e                                            { $$ = mk_app(mk_app(mk_op(DIV),$1),$3);}
	| e T_MULT e                                           { $$ = mk_app(mk_app(mk_op(MULT),$1),$3);}
	| e T_LEQ e                                            { $$ = mk_app(mk_app(mk_op(LEQ),$1),$3) ;}
	| e T_LE e                                             { $$ = mk_app(mk_app(mk_op(LE),$1),$3) ;}
	| e T_GEQ e                                            { $$ = mk_app(mk_app(mk_op(GEQ),$1),$3) ;}
	| e T_GE e                                             { $$ = mk_app(mk_app(mk_op(GE),$1),$3) ;}
	| e T_OR e                                             { $$ = mk_app(mk_app(mk_op(OR),$1),$3) ;}
	| e T_AND e                                            { $$ = mk_app(mk_app(mk_op(AND),$1),$3) ;}
	| e T_EQ e                                             { $$ = mk_app(mk_app(mk_op(EQ),$1),$3) ;}
	| T_NOT e[expr]                                        { $$ = mk_app(mk_op(NOT),$expr) ;}
	
	/*Reconnaissance d'identificateurs et de variables*/
	| T_ID                                                 { $$ = mk_id($1);}

	/* Définition fonction */
	| T_FUN T_ID[var] arg_list[expr]                       { $$ = mk_fun($var,$expr);}

	/* LET IN */
	| T_LET T_ID[x] T_EQUAL e[arg] T_IN e[exp]             { $$ = mk_app(mk_fun($x,$exp),$arg); }

	/* WHERE */
	| e[exp] T_WHERE T_ID[x] T_EQUAL e[arg]                { $$ = mk_app(mk_fun($x,$exp),$arg); }

	/* IF THEN ELSE */
	| T_IF e[cond] T_THEN e[then_br] T_ELSE e[else_br]     { $$ = mk_cond($cond, $then_br, $else_br) ;}

	/* op sur les listes + liste */
	| T_POP e[l]					       { $$ = mk_app(mk_op(POP),$l);}
	| T_NEXT e[l]					       { $$ = mk_app(mk_op(NEXT),$l);}
	| '[' list[l] ']'                                      { $$ = $l;}
	| e[exp] T_PUSH e[l]                                   { $$ = mk_app(mk_app(mk_op(PUSH),$exp),$l);}

	/* Composition de fonction */
	|  e[fun] e[arg] %prec FUNCTION_APPLICATION            { $$ = mk_app($fun,$arg);}

	/* Ignorer les parenthèses inutiles */
	| '(' e ')'                                            { $$ = $2;}
	;



/* Définition de fonction à argument multiples */
arg_list:T_ARROW e                                             { $$=$2;}
|T_ID[var] arg_list                                            { $$=mk_fun ($1, $2); }
;

/* LISTES */
list	: e[ex]					{$$ = mk_cell($ex,mk_nil());}
	| e[ex] ',' list[l] 	                {$$ = mk_cell($ex,$l);}
	|	      				{$$ = mk_nil();}
	;

%%


/*******************************/
/********* MAIN ****************/
/*******************************/
	
int main(int argc, char *argv[])
{
  //extern int yydebug;
  //yydebug = 1;

  yyparse();

  return EXIT_SUCCESS;
}
