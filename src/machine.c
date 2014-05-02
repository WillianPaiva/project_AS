
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "../interface/machine.h"
#include <math.h>

#define PI 3.14159265

#define MAX_CLOSURE 1000000

int nb_closure;

int get_num(struct closure *cl){
  int res =  cl->expr->expr->num;
  return res;
} 

struct expr * get_point(struct closure *cl){
  struct expr *res =  cl->expr;
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


struct expr *id(struct expr *id, struct env *env){
	struct closure *cl =  search_env(id->expr->id,env);
    return cl->expr;
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
void html_head(FILE *f){

	fprintf(f,"<!DOCTYPE HTML>\n");
	fprintf(f,"<html>\n");
	fprintf(f,"<head>\n");
	fprintf(f,"<script type=\"text/javascript\">\n");
	fprintf(f,"function drawShape(){\n");
	fprintf(f,"		var canvas = document.getElementById('mycanvas');\n");
	fprintf(f,"		canvas.width = 800\n");
	fprintf(f,"		canvas.height = 800\n");
	fprintf(f,"		if (canvas.getContext){\n");
	fprintf(f,"			var ctx = canvas.getContext('2d');\n");
	
}


void html_tail(FILE *f){

	
	fprintf(f,"		} else {\n");
	fprintf(f,"			alert('You need Safari or Firefox 1.5+ to see this demo.');\n");  
	fprintf(f,"		}\n");
	fprintf(f,"	}\n");
	fprintf(f,"</script>\n");
	fprintf(f,"</head>\n");
	fprintf(f,"<body onload=\"drawShape();\">\n");
	fprintf(f,"		<canvas id=\"mycanvas\"></canvas>\n");
	fprintf(f,"</body>\n");
	fprintf(f,"</html>\n");


}

void draw_path(struct expr * dr, struct env * env,FILE *f){
  struct expr *p1 = dr->expr->path.point;
  while(p1->type == ID){
		p1 = id(p1,env);	
	}

	int x = p1->expr->point.x->expr->num;
    int y = p1->expr->point.y->expr->num;
	fprintf(f,"			ctx.beginPath();\n");
	
	fprintf(f,"			ctx.moveTo(%d,%d);\n",x,y);
	struct expr *next = dr->expr->path.next;
	while(next){
		p1 = next->expr->path.point;
		while(p1->type == ID){
			p1 = id(p1,env);	
		}
		x = p1->expr->point.x->expr->num;
		y = p1->expr->point.y->expr->num;
	
			fprintf(f,"			ctx.lineTo(%d,%d);\n",x,y);
			next = next->expr->path.next;
		
	}	
	fprintf(f,"			ctx.stroke();\n");
		

}

void draw_circle(struct expr * dr,struct env *env,FILE *f){
	struct expr *p1 = dr->expr->circle.center;
	while(p1->type == ID){
		p1 = id(p1,env);	
	}
	struct expr *p3 = dr->expr->circle.radius;
	while(p3->type == ID){
		p3 = id(p3,env);	
	}
	assert(p1->type == POINT);
	assert(p3->type == NUM);


	int x = p1->expr->point.x->expr->num;
    int y = p1->expr->point.y->expr->num;
	int r = p3->expr->num;



	fprintf(f,"			ctx.beginPath();\n");

	fprintf(f,"			ctx.arc(%d,%d,%d,0,2*Math.PI);\n",x,y,r);

	fprintf(f,"			ctx.stroke();\n");


	

}


void draw_bezier(struct expr * dr,struct env *env, FILE *f){
   	struct expr *p1 = dr->expr->bezier.pt1;
	while(p1->type == ID){
		p1 = id(p1,env);	
	}
	assert(p1->type == POINT);

	int x1 = p1->expr->point.x->expr->num;
    int y1 = p1->expr->point.y->expr->num;




	fprintf(f,"			ctx.beginPath();\n");


	fprintf(f,"			ctx.moveTo(%d,%d);\n",x1,y1);
	fprintf(f,"			ctx.bezierCurveTo(");
	struct expr *next = dr->expr->bezier.next;
	int x;
	int y;
	while(next){
		p1 = next->expr->bezier.pt1;
		while(p1->type == ID){
			p1 = id(p1,env);	
		}
		assert(p1->type == POINT);
		x = p1->expr->point.x->expr->num;
		y = p1->expr->point.y->expr->num;
		if(next->expr->path.next){
			fprintf(f,"%d,%d,",x,y);
		}else{
		
			fprintf(f,"%d,%d",x,y);

		}			
			next = next->expr->bezier.next;
		
	}
	fprintf(f,");\n");
	
	fprintf(f,"			ctx.stroke();\n");


	

}

void draw(struct expr * dr, struct env *env,FILE *f){
	char test[12][12] = {"ID", "FUN", "APP", "NUM", "OP", "COND", "CELL", "NIL", "POINT", "PATH", "CIRCLE","BEZIER"};
	switch(dr->type){
	case PATH:{
	  draw_path(dr,env,f);
	  return;
		}
	case CIRCLE:{
	  draw_circle(dr,env,f);
	  return;
	}
	case BEZIER:{
	  draw_bezier(dr,env,f);
	  return;
	}
	case CELL:{
	  struct expr *next = dr->expr->cell.next;
	  struct expr *image = dr->expr->cell.ex;
	  
	  while(image->type == ID){
	    image = id(image,env);			
	  }
	  
	  
	  draw(image,env,f);
	  if(next){
	    draw(next,env,f);
	  }
	  
	  return;
	}
	case NIL:{
	  return;
	}
	default:{
	  printf("------->%s is not a drawble object\n",test[dr->type]);
	  return;
	}
	  
	}
	
}

void map(struct expr * dr, struct env *env){
	char * name = "output.html";
	FILE *f;
	f = fopen(name,"w");
	if(f == NULL){	
		f = fopen(name,"wb");
	}
	html_head(f);
	draw(dr,env,f);
	html_tail(f);
	fclose(f);
	printf("output.html created\n");


}

struct expr* rotate_point(struct expr* p, struct expr* v, int angle, struct env *env)
{ 
	  assert(p->type == POINT && v->type == POINT);

	  struct expr * px = p->expr->point.x;
	  while(px->type == ID){
				px = id(px,env);
				
		}

	  struct expr * py = p->expr->point.y;
	  while(py->type == ID){
				py = id(py,env);
				
	  }
	  struct expr * vx = v->expr->point.x;
	  while(vx->type == ID){
				vx = id(vx,env);
				
		}

	  struct expr * vy = v->expr->point.y;
	  while(vy->type == ID){
				vy = id(vy,env);
				
	  }
	int refx = vx->expr->num;
	int refy =vy->expr->num;
	int x = px->expr->num ;
	int y =	py->expr->num;
	float theta = angle*(PI/180);
	return mk_point(mk_int(refy + (x-refx)*cos(theta)-(y-refy)*sin(theta)),mk_int(refy + (x-refx)*sin(theta)+(y-refy)*cos(theta)));

}

struct expr* trans_point(struct expr* p, struct expr* v, struct env *env){
  assert(p->type == POINT && v->type == POINT);

  struct expr * px = p->expr->point.x;
  while(px->type == ID){
			px = id(px,env);
			
	}

  struct expr * py = p->expr->point.y;
  while(py->type == ID){
			py = id(py,env);
			
  }
  struct expr * vx = v->expr->point.x;
  while(vx->type == ID){
			vx = id(vx,env);
			
	}

  struct expr * vy = v->expr->point.y;
  while(vy->type == ID){
			vy = id(vy,env);
  }




  





  return mk_point(mk_int(px->expr->num +  vx->expr->num),  mk_int(py->expr->num +  vy->expr->num));

}

struct expr *translater(struct expr* fig, struct expr* vect, struct env *env){
  	struct expr * p1 ;
	struct expr *next;
	struct expr *newfig	;

	char test[12][12] = {"ID", "FUN", "APP", "NUM", "OP", "COND", "CELL", "NIL", "POINT", "PATH", "CIRCLE","BEZIER"};


	switch (fig->type){

	  case POINT:
		newfig =  trans_point(fig,vect,env);
		return newfig;
	  case PATH:
		   p1 = fig->expr->path.point;
		  
			while(p1->type == ID){
				p1 = id(p1,env);
			}
			
			newfig = mk_path(trans_point(p1,vect,env),NULL);
		
		next = fig->expr->path.next;

		while(next){
			p1 = next->expr->path.point;
			 
			while(p1->type == ID){
				p1 = id(p1,env);
				
			}
			newfig = mk_path(trans_point(p1,vect,env),newfig);
			next = next->expr->path.next;

		
		}

		return newfig;	

	  case CIRCLE:
			p1 = fig->expr->circle.center;
		   
			while(p1->type == ID){
				p1 = id(p1,env);
			
			}

			
			newfig = mk_circle(trans_point(p1,vect,env),fig->expr->circle.radius);
			return newfig;

	  case BEZIER:
			p1 = fig->expr->bezier.pt1;
			while(p1->type == ID){
				p1 = id(p1,env);
			}

			newfig = mk_bezier(trans_point(p1,vect,env),NULL);
			
			next = fig->expr->bezier.next;

			while(next){
				p1 = next->expr->bezier.pt1;
				while(p1->type == ID){
					p1 = id(p1,env);
				}
			    
				newfig = mk_bezier(trans_point(p1,vect,env),newfig);
				next = next->expr->bezier.next;

			
			}

			return newfig;	
  
		default:
			printf("------->%s is not a image object\n",test[fig->type]);
			return fig;


  
  }
}

struct expr *rotate(struct expr* fig, struct expr* vect,int angle, struct env *env){
  	struct expr * p1 ;
	struct expr *next;
	struct expr *newfig	;

	char test[12][12] = {"ID", "FUN", "APP", "NUM", "OP", "COND", "CELL", "NIL", "POINT", "PATH", "CIRCLE","BEZIER"};

	switch (fig->type){

	  case POINT:
	    newfig =  rotate_point(fig,vect,angle,env);
		return newfig;
	  case PATH:
		   p1 = fig->expr->path.point;
		  
			while(p1->type == ID){
				p1 = id(p1,env);
			}
			
			newfig = mk_path(rotate_point(p1,vect,angle,env),NULL);
		
		next = fig->expr->path.next;

		while(next){
			p1 = next->expr->path.point;
			 
			while(p1->type == ID){
				p1 = id(p1,env);
				
			}
			newfig = mk_path(rotate_point(p1,vect,angle,env),newfig);
			next = next->expr->path.next;

		
		}

		return newfig;	

	  case CIRCLE:
			p1 = fig->expr->circle.center;
		   
			while(p1->type == ID){
				p1 = id(p1,env);
			
			}

			
			newfig = mk_circle(rotate_point(p1,vect,angle,env),fig->expr->circle.radius);
			return newfig;

	  case BEZIER:
			p1 = fig->expr->bezier.pt1;
			while(p1->type == ID){
				p1 = id(p1,env);
			}

			newfig = mk_bezier(rotate_point(p1,vect,angle,env),NULL);
			
			next = fig->expr->bezier.next;

			while(next){
				p1 = next->expr->bezier.pt1;
				while(p1->type == ID){
					p1 = id(p1,env);
				}
			    
				newfig = mk_bezier(rotate_point(p1,vect,angle,env),newfig);
				next = next->expr->bezier.next;

			
			}

			return newfig;	
  
		default:
			printf("------->%s is not a image object\n",test[fig->type]);
			return fig;


  
  }
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
  case POINT:
    return;
  case PATH:
    return;
  case CIRCLE:
    return;
  case BEZIER:
    return;

  case OP:
    {
     struct stack *stack = conf->stack;
     if(stack==NULL){return;}
     struct closure *arg1 = stack->closure;
     stack = pop_stack(stack);
     switch(expr->expr->op){
	 case DRAW:
		 eval_arg(conf,arg1);
		 map(conf->closure->expr ,conf->closure->env);
		 return;
     case NOT: 
       eval_arg(conf,arg1);
       conf->closure->expr->expr->num = !get_num(conf->closure);
       return;
     case POP:
       eval_arg(conf,arg1);
       conf->closure = mk_closure(get_cell(conf->closure).ex,conf->closure->env);
       conf->stack=stack;
       step(conf);
       return;
     case NEXT:
       eval_arg(conf,arg1);
       conf->closure = mk_closure(get_cell(conf->closure).next,conf->closure->env);
       return;
     }
     if(stack==NULL){return;}
     struct closure *arg2 = stack->closure;
     stack = pop_stack(stack);
	 struct expr *t1;
	 struct expr *t2;

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
     case PUSH: 
       conf->closure = mk_closure(mk_cell(arg1->expr,arg2->expr),arg1->env);
       return;
     case TRANS:
	   eval_arg(conf,arg1);
       t1 = get_point(conf->closure);
       eval_arg(conf,arg2);
       t2 = get_point(conf->closure);
       conf->closure = mk_closure(translater(t1,t2,conf->closure->env),NULL);
       return;
     }
     if(stack==NULL){return;}
     struct closure *arg3 = stack->closure;
     stack = pop_stack(stack);
     int t3;
     switch (expr->expr->op){
     case ROT:
       eval_arg(conf,arg1);
       t1 = get_point(conf->closure);
       eval_arg(conf,arg2);
       t2 = get_point(conf->closure);
       eval_arg(conf,arg3);
       t3 = get_num(conf->closure);    
       conf->closure = mk_closure(rotate(t1,t2,t3,conf->closure->env),NULL);
       return;  
     default: assert(0);
     }   
    }
    ;
  default: assert(0);
  }
}
