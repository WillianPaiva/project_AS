
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
