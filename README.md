# Projet d'analyse syntaxique

## Table des matières

  * 1. Modalités
  * 2. Constructions de base
  * 3. Sémantique opérationnelle
  * 4. Travail demandé (première partie)

Nous proposons de construire un langage de programmation fonctionnelle qui
sera ensuite spécialisé pour construire des images ou pour écrire de la
musique.

## 1 Modalités

Le projet est à rendre en deux parties. La première partie décrite ici demande
d'analyser un langage de programmation fonctionnel pour l'exécuter sur une
machine virtuelle fournie. La seconde partie ajoutera des capacités graphiques
et/ou musicales.

Dates de remise du projet (attention, dates **strictes**) :

  * 1ère partie : Lundi 7 avril 2014, 12h00. 
  * 2ème partie : Lundi 5 mai 2014, 12h00. 

Le projet est à réaliser par groupes de 3 ou 4 étudiant(e)s. Chaque membre
doit contribuer de façon significative au travail de programmation, avoir
connaissance des options choisies, des difficultés rencontrées, et des
solutions retenues, et exposera sa contribution individuellement lors des
soutenances (prévues les 12 et 13 mai, dates à confirmer). On demande
également un rapport court (5 à 10 pages) illustré par des exemples, exposant
les problèmes rencontrés et les choix effectués.

La notation tiendra compte du degré de réalisation du sujet, de la qualité
du code (portabilité, lisibilité, documentation), du jeu de tests présenté,
et de la qualité du rapport et de la soutenance. Elle peut varier à
l’intérieur d’un groupe en cas de travail trop inégal.

## 2 Constructions de base

Le langage que nous proposons d'implémenter sera structuré par une série de
déclarations de la forme :

    
    let id = expr;
    

et de commandes de la forme :

    
    expr;
    

Les expressions seront les suivantes -- les parenthèses font partie de la
description :

  1. des identifiants, 
  2. des constructions de fonctions : 
    
    (fun x -> expr)
    

  3. des applications de fonctions : 
    
    (f g)
    

  4. des expressions arithmétiques écrites à partir d'opérateurs habituels en notation infixe, d'identificateurs, de fonctions et de constantes entières. 
  5. des conditionnelles : 
    
    (if expr then expr else expr)
    

La structure syntaxique du programme sera reflétée par les structures C,
mutuellement récursives, décrites dans le fichier [expr.h](expr.h).

    
    enum expr_kind {ID, FUN, APP, NUM, OP, COND};
    
    enum op{PLUS, MINUS, MULT, DIV, LEQ, LE, GEQ, GE, EQ, OR, AND, NOT};
    
    struct expr;
    
    struct fun{
      char *id;
      struct expr *body;
    };
    
    struct app{
      struct expr *fun;
      struct expr *arg;
    };
    
    struct cond{
      struct expr *cond;
      struct expr *then_br;
      struct expr *else_br;
    };
    
    union node{
      char *id;
      struct fun fun;
      struct app app;
      enum op op;
      int num;
      struct cond cond;
    };
    
    struct expr{
      enum expr_kind type;
      union node *expr;
    };
    

On ajoute également un certain nombre de fonctions qui permettent de
construire la structure syntaxique des expressions. Ces fonctions sont
déclarées dans le fichier [expr.h](expr.h) :

    
    struct expr *mk_node(void);
    struct expr *mk_id(char *id);
    struct expr *mk_fun(char *id, struct expr *body);
    struct expr *mk_app(struct expr *fun, struct expr *arg);
    struct expr *mk_op(enum op op);
    struct expr *mk_int(int k);
    struct expr *mk_cond(struct expr *cond, struct expr *then_br, struct expr *else_br);
    

et définies dans le fichier [expr.c](expr.c) :

    
    #include <stdlib.h>
    #include "expr.h"
    
    struct expr *mk_node(void){
      struct expr *e = malloc(sizeof(struct expr));
      e->expr = malloc(sizeof(union node));
      return e;
    }
    
    struct expr *mk_id(char *id){
      struct expr *e = mk_node();
      e->type = ID;
    
      e->expr->id = id;
      return e;
    }
    
    struct expr *mk_fun(char *id, struct expr *body){
      struct expr *e = mk_node();
      e->type = FUN;
      e->expr->fun.id = id;
      e->expr->fun.body = body;
      return e;
    }
    
    struct expr *mk_app(struct expr *fun, struct expr *arg){
      struct expr *e = mk_node();
      e->type = APP;
      e->expr->app.fun=fun;
      e->expr->app.arg=arg;
      return e;
    }
    
    struct expr *mk_op(enum op op){
      struct expr *e = mk_node();
      e->type=OP;
      e->expr->op = op;
      return e;
    }
    
    struct expr *mk_int(int k){
      struct expr *e = mk_node();
      e->type=NUM;
      e->expr->num = k;
      return e;
    }
    
    struct expr *mk_cond(struct expr *cond, struct expr *then_br, struct expr *else_br){
      struct expr *e = mk_node();
      e->type = COND;
      e->expr->cond.cond = cond;
      e->expr->cond.then_br = then_br;
      e->expr->cond.else_br = else_br;
      return e;
    }
    

Les fonctions précédentes permettent de construire l'arbre syntaxique
correspondant à une expression.

## 3 Sémantique opérationnelle

La sémantique opérationnelle de ce langage sera donnée par une machine à
environnement. Cette machine comporte un programme, un environnement, et une
pile. La pile sera utilisée pour l'évaluation d'expression. L'environnement
permet de mémoriser les liaisons entre les identificateurs et leurs valeurs.
Par exemple, après la déclaration

    
    let x = 7;
    

l'environnement contiendra la liaison entre l'identificateur `x` et la
l'expression constante `7`. De même, après

    
    let double = fun z -> 2*z;
    

il faut ajouter dans l'environnement la liaison entre l'identificateur
`double` et l'expression `fun z -> 2*z` qui définit cette fonction.

Si l'on écrit ensuite

    
    x + 3;
    

la recherche de la valeur de `x` dans l'environnement permettra d'évaluer
cette expression à `10`.

Par contre, une nouvelle déclaration

    
    let y = x + 3;
    

ne relie pas l'identificateur `y` et la valeur 10 : l'évaluation sera
effectuée uniquement au moment où on en aura besoin. Dans l'environnement, on
ajoute donc la liaison entre l'identificateur `y` et **l'expression** `x+3`.
Cela a pour conséquence que l'on doit aussi mémoriser, dans la même structure,
quelle est la valeur de `x` au moment où `y` a été définie. Dans cet exemple,
cette valeur est `3`, et en général, cette valeur peut être retrouvée grâce à
l'environnement courant. Ce que l'on ajoute donc à l'environnement est la
liaison entre `y` et **l'expression `x+3` avec l'environnement courant**. Une
expression avec un environnement s'appelle une _clôture_.

Le même principe s'applique quelle que soit l'expression, par exemple une
expression fonctionnelle. Ainsi, après

    
    let f = fun t -> double (double t) + y;
    

l'expression ci-dessus `fun t -> ...`, liée au nom `f` dépend de
l'environnement courant qui contient `double` et `y`. Pour cette raison, on
mémorise donc la clôture contenant à la fois l'expression qui définit `f`,
mais également l'environnement courant.

Cette mémorisation d'une clôture se fait en empilant la clôture calculée sur
l'environnement courant, par la fonction `push_rec_env` qui vous est fournie.

Les structures sont les suivantes, fournies dans le fichier
[machine.h](machine.h).

    
    #include "expr.h"
    
    struct env;
    struct closure;
    struct configuration;
    struct stack;
    
    struct env{
      char *id;
      struct closure *closure;
      struct env *next;
    };
    
    struct closure{
      struct expr *expr;
      struct env *env;
    };
    
    struct configuration{
      struct closure *closure;
      struct stack *stack;
    };
    
    struct stack{
      struct closure *closure;
      struct stack *next;
    };
    
    struct closure *mk_closure(struct expr *expr, struct env *env);
    struct configuration *mk_conf(struct closure *cl);
    struct env *push_env(char *id, struct closure *cl, struct env *env);
    struct env *push_rec_env(char *id, struct expr *expr, struct env *env);
    void step(struct configuration *conf);
    

Enfin, pour calculer les expressions données par les commandes de la forme :

    
    expr;
    

on utilise à la fois l'environnement, qui contient les valeurs des variables,
mais aussi la pile. Pour cela, on part de la clôture correspondant à
l'expression `expr` à calculer dans l'environnement courant, et de la pile
vide (une clôture et une pile constituent une _configuration_). La pile sert
par exemple à évaluer des applications de fonctions à des arguments. La
fonction permettant de faire cette évaluation à partir d'une configuration
vous est également fournie, il s'agit de la fonction `step`.

Les fonctions permettant de manipuler l'environnement, la pile, les clôtures
sont les suivantes, fournies dans le fichier [machine.c](machine.c).

    
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>
    #include <assert.h>
    #include "machine.h"
    
    #define MAX_CLOSURE 1000000
    
    int nb_closure;
    
    struct closure *mk_closure(struct expr *expr, struct env *env){
      assert(nb_closure<MAX_CLOSURE);
      struct closure *cl = malloc(sizeof(struct closure));
      cl->expr = expr;
      cl->env = env;
      nb_closure++;
      return cl;
    }
    
    struct configuration *mk_conf(struct closure *cl){
      struct configuration *conf = malloc(sizeof(struct configuration));
      conf->closure = cl;
      conf->stack=NULL;
      return conf;
    }
    
    void print_env(const struct env *env){
      if(env==NULL){printf(" #\n");}
      else{printf(" %s ",env->id);print_env(env->next);}
    }
    
    struct env *push_env(char *id, struct closure *cl, struct env *env){
      struct env *e = malloc(sizeof(struct env));
      e->id = id;
      e->closure = cl;
      e->next = env;
      //print_env(e);
      return e;
    }
    
    struct env *push_rec_env(char *id, struct expr *expr, struct env *env){
      struct env *e = malloc(sizeof(struct env));
      struct closure *cl = mk_closure(expr,e);
      e-> id = id;
      e->closure = cl;
      e->next = env;
      //print_env(e);
      return e;
    }
    
    struct closure *search_env(char *id, struct env *env){
      assert(env!=NULL);
      if(strcmp(id,env->id)==0){return env->closure;}
      else{return search_env(id,env->next);}  
    }
    
    struct stack *pop_stack(struct stack *stack){
      struct stack *next = stack->next;
      free(stack);
      return next;
    }
    
    struct stack *push_stack(struct closure *cl, struct stack *stack){
      struct stack *st = malloc(sizeof(struct stack));
      st->closure = cl;
      st->next = stack;
      return st;
    }
    
    void step(struct configuration *conf){
      struct expr *expr = conf->closure->expr;
      struct env *env = conf->closure->env;
      struct stack *stack = conf->stack;
      assert(expr!=NULL);
      switch (expr->type){
      case FUN: 
        {// printf("fun\n");
          if(stack==NULL){return;}
          char *var = expr->expr->fun.id;
          struct expr *body = expr->expr->fun.body;
          struct env *e = push_env(var, stack->closure,env);
          conf->closure=mk_closure(body,e);
          conf->stack = pop_stack(stack);
          return step(conf);
        }
      case APP: 
        { 
          struct expr *fun = expr->expr->app.fun;
          struct expr *arg = expr->expr->app.arg;
          conf->closure = mk_closure(fun,env);
          conf->stack = push_stack(mk_closure(arg,env),conf->stack);
          return step(conf);
        }
      case ID: //printf("id: %s\n", expr->expr->id);
        assert(env!=NULL);
        conf->closure = search_env(expr->expr->id,env);
        return step(conf);
      case COND:
        { 
          struct stack *stack = conf->stack;
          struct closure *cl_cond = mk_closure(expr->expr->cond.cond,env);
          conf->closure = cl_cond;
          conf->stack=NULL;
          step(conf);
          assert(conf->closure->expr->type==NUM);
          conf->stack=stack;
          if(conf->closure->expr->expr->num==0){
            //printf("cond false\n");
            conf->closure = mk_closure(expr->expr->cond.else_br,env);
          }
          else{
            //printf("cond false\n");
            conf->closure = mk_closure(expr->expr->cond.then_br,env);
          }
          return step(conf);
        }
      case NUM: 
        return;
      case OP: 
        {
         struct stack *stack = conf->stack;
         if(stack==NULL){return;}
         struct closure *arg1 = stack->closure;
         stack = pop_stack(stack);
         conf->closure = arg1;
         conf->stack = NULL;
         step(conf);
         if(conf->closure->expr->type!=NUM){exit(EXIT_FAILURE);}
         int k1 = conf->closure->expr->expr->num;
         if(conf->closure->expr->expr->op==NOT){
           conf->closure->expr->expr->num = !k1;
           return;
         }
         if(stack==NULL){return;}
         arg1=conf->closure;
         struct closure *arg2 = stack->closure;
         stack = pop_stack(stack);
         conf->closure = arg2;
         conf->stack=NULL;
         step(conf);
         if(conf->closure->expr->type!=NUM){exit(EXIT_FAILURE);}
         int k2 = conf->closure->expr->expr->num;
         switch (expr->expr->op){
         case PLUS: //printf("plus\n");
           conf->closure = mk_closure(mk_int(k1+k2),NULL);return;
         case MINUS: //printf("minus\n");
           conf->closure = mk_closure(mk_int(k1-k2),NULL);return;
         case MULT: //printf("mult\n");
           conf->closure = mk_closure(mk_int(k1*k2),NULL);return;
         case DIV: assert(k2!=0); conf->closure =  mk_closure(mk_int(k1-k2),NULL);return;
         case LEQ: //printf("%d <= %d \n",k1,k2);
           conf->closure = mk_closure(mk_int(k1 <= k2),NULL); return;
         case LE: conf->closure = mk_closure(mk_int(k1 < k2),NULL); return;
         case GEQ: conf->closure = mk_closure(mk_int(k1 >= k2),NULL); return;
         case GE: conf->closure = mk_closure(mk_int(k1 > k2),NULL); return;
         case EQ: conf->closure = mk_closure(mk_int(k1 == k2),NULL); return;
         case OR: conf->closure = mk_closure(mk_int(k1 || k2),NULL); return;
         case AND: conf->closure = mk_closure(mk_int(k1 && k2),NULL); return;
         default: assert(0);
         }   
       }
       ;
      default: assert(0);
      }
    }
    

## 4 Travail demandé (première partie)

  1. Écrire un analyseur syntaxique pour le langage décrit plus haut permettant 
    * de construire l'arbre syntaxique correspondant à une expression, en utilisant les constructeurs `mk_*` fournis dans le fichier [expr.c](expr.c). 
    * d'exécuter un programme sur la machine fournie, en 
      * affichant, après une expression complète terminée par le symbole `;`, la valeur de cette expression si elle entière. 
      * ajoutant le traitement les déclarations `let x = ...`. 
  2. Rendre le langage plus simple d'utilisation en éliminant des structures inutiles : 
    * permettre d'écrire `(fun x1 ... xn -> expr)` pour `(fun x1 -> ... (fun xn -> expr)...)`. 
    * permettre d'écrire `(f g1 ... gn)` pour `(...(f g1)... gn)`. 
    * permettre d'enlever les parenthèses autour d'une expression si cela ne crée pas d'ambiguïté. Par exemple : 
    
    let f = fun x1 ... xn -> if c then e1 else e2;
    

à la place de

    
    let f = (fun x1 -> (... (fun xn -> (if c then e1 else e2))...));
    

  3. (optionel) Ajouter des constructions pour les expressions des langages fonctionnels : 
    * `let in`
    * `where`
  4. (optionel) Ajouter la manipulation de listes. 

