
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "../interface/machine.h"

#define MAX_CLOSURE 1000000

int nb_closure;

int get_num(struct closure *cl){
  int res =  cl->expr->expr->num;
  return res;
} 

void eval_arg(struct configuration *conf, struct closure *arg){
  conf->closure = arg;
  conf->stack = NULL;
  step(conf);
}

struct cell get_cell(struct closure * cl){
  assert(cl->expr->type == CELL);
  return cl->expr->expr->cell;
}

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
    {
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
  case ID: 
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
        
        conf->closure = mk_closure(expr->expr->cond.else_br,env);
      }
      else{
        
        conf->closure = mk_closure(expr->expr->cond.then_br,env);
      }
      return step(conf);
    }
  case NUM: 
    return;
  case NIL:
    return;
  case CELL:
    return;
  case OP: 
    {
     struct stack *stack = conf->stack;
     if(stack==NULL){return;}
     struct closure *arg1 = stack->closure;
     stack = pop_stack(stack);
     struct expr *e1 = conf->closure->expr;
     switch(expr->expr->op){
     case NOT: 
       eval_arg(conf,arg1);
       conf->closure->expr->expr->num = !get_num(conf->closure);
       return;
     case HEAD:
       eval_arg(conf,arg1);
       conf->closure->expr = get_cell(conf->closure).ex;
       conf->stack=stack;
       step(conf);
       return;
     case TAIL:
       eval_arg(conf,arg1);
       conf->closure->expr = get_cell(conf->closure).next;
       conf->stack=stack;
       step(conf);
       return;
     }
     if(stack==NULL){return;}
     struct closure *arg2 = stack->closure;
     stack = pop_stack(stack);
     struct expr *e2 = conf->closure->expr;
     int k1,k2;
     switch (expr->expr->op){
     case PLUS: 
       eval_arg(conf,arg1);
       k1 = get_num(conf->closure);
       eval_arg(conf,arg2);
       k2 = get_num(conf->closure);
       conf->closure = mk_closure(mk_int(k1+k2),NULL);return;
     case MINUS: 
       eval_arg(conf,arg1);
       k1 = get_num(conf->closure);
       eval_arg(conf,arg2);
       k2 = get_num(conf->closure);
       conf->closure = mk_closure(mk_int(k1-k2),NULL);return;
     case MULT: 
       eval_arg(conf,arg1);
       k1 = get_num(conf->closure);
       eval_arg(conf,arg2);
       k2 = get_num(conf->closure);
       conf->closure = mk_closure(mk_int(k1*k2),NULL);return;
     case DIV:  
       eval_arg(conf,arg1);
       k1 = get_num(conf->closure);
       eval_arg(conf,arg2);
       k2 = get_num(conf->closure);
       assert(k2!=0);
       conf->closure =  mk_closure(mk_int(k1/k2),NULL);return;
     case LEQ: 
       eval_arg(conf,arg1);
       k1 = get_num(conf->closure);
       eval_arg(conf,arg2);
       k2 = get_num(conf->closure);
       conf->closure = mk_closure(mk_int(k1 <= k2),NULL); return;
     case LE: 
       eval_arg(conf,arg1);
       k1 = get_num(conf->closure);
       eval_arg(conf,arg2);
       k2 = get_num(conf->closure);
       conf->closure = mk_closure(mk_int(k1 < k2),NULL); return;
     case GEQ:
       eval_arg(conf,arg1);
       k1 = get_num(conf->closure);
       eval_arg(conf,arg2);
       k2 = get_num(conf->closure);
       conf->closure = mk_closure(mk_int(k1 >= k2),NULL); return;
     case GE: 
       eval_arg(conf,arg1);
       k1 = get_num(conf->closure);
       eval_arg(conf,arg2);
       k2 = get_num(conf->closure);
       conf->closure = mk_closure(mk_int(k1 > k2),NULL); return;
     case EQ: 
       eval_arg(conf,arg1);
       k1 = get_num(conf->closure);
       eval_arg(conf,arg2);
       k2 = get_num(conf->closure);
       conf->closure = mk_closure(mk_int(k1 == k2),NULL); return;
     case OR: 
       eval_arg(conf,arg1);
       k1 = get_num(conf->closure);
       eval_arg(conf,arg2);
       k2 = get_num(conf->closure);
       conf->closure = mk_closure(mk_int(k1 || k2),NULL); return;
     case AND: 
       eval_arg(conf,arg1);
       k1 = get_num(conf->closure);
       eval_arg(conf,arg2);
       k2 = get_num(conf->closure);
       conf->closure = mk_closure(mk_int(k1 && k2),NULL); return;
     case CONS: 
       conf->closure = mk_closure(mk_cell(arg1->expr,arg2->expr),arg1->env);
       return;
     default: assert(0);
     }   
   }
   ;
  default: assert(0);
  }
}
