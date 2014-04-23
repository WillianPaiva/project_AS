
#include <stdlib.h>
#include <stdio.h>
#include "../interface/expr.h"


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


struct expr *mk_cell(struct expr* expr, struct expr* next){
  struct expr* e = mk_node();
  e->type = CELL;
  e->expr->cell.ex = expr;
  e->expr->cell.next = next;
  return e;
  
}

struct expr *mk_point(struct expr* x, struct expr* y){
  struct expr* e = mk_node();
  e->type = POINT;
  e->expr->point.x = x;
  e->expr->point.y = y;
  return e;
  
}

struct expr *mk_path(struct expr* point, struct expr* next){
  	struct expr* e;
	
	if(point->type == PATH){
		if(point->expr->path.next){
			e = point->expr->path.next;
			while(e->expr->path.next){
				e = e->expr->path.next;
			}
			e->expr->path.next = next;
			return point;
		}else{
			
			point->expr->path.next = next;
			return point;
		}
  
  }else{
	
	  e = mk_node();
	  e->type = PATH;
	  e->expr->path.point = point;
	  e->expr->path.next = next;
	  return e;
   }
}

struct expr *mk_circle(struct expr* center, struct expr* radius){
  struct expr* e = mk_node();
  e->type = CIRCLE;
  e->expr->circle.center = center;
  e->expr->circle.radius = radius;
  return e;
  
}


struct expr * mk_nil(){
  struct expr* e = mk_node();
  e->type = NIL;
  e->expr = NULL;
  return e;
}
