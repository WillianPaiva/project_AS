
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
