
enum expr_kind {ID, FUN, APP, NUM, OP, COND, CELL, NIL, POINT, PATH, CIRCLE};

enum op{PLUS, MINUS, MULT, DIV, LEQ, LE, GEQ, GE, EQ, OR, AND, NOT, PUSH, POP, NEXT};

struct expr;

struct cell {
  struct expr* ex;
  struct expr* next;
};


struct point {
  struct expr* x;
  struct expr* y;
};

struct path {
  struct expr* point;
  struct expr* next;
};


struct circle {
  struct expr* center;
  struct expr* radius;
};





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
  struct cell cell;
  struct point point;
  struct path path;
  struct circle circle;

};

struct expr{
  enum expr_kind type;
  union node *expr;
};

struct expr *mk_node(void);
struct expr *mk_id(char *id);
struct expr *mk_fun(char *id, struct expr *body);
struct expr *mk_app(struct expr *fun, struct expr *arg);
struct expr *mk_op(enum op op);
struct expr *mk_int(int k);
struct expr *mk_cond(struct expr *cond, struct expr *then_br, struct expr *else_br);
struct expr *mk_cell(struct expr *expr, struct expr *next);
struct expr *mk_point(struct expr *x, struct expr *y);
struct expr *mk_nil(void);
struct expr *mk_path(struct expr *point, struct expr *next);
struct expr *mk_circle(struct expr *center, struct expr *radius);



