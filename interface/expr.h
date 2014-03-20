
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

struct expr *mk_node(void);
struct expr *mk_id(char *id);
struct expr *mk_fun(char *id, struct expr *body);
struct expr *mk_app(struct expr *fun, struct expr *arg);
struct expr *mk_op(enum op op);
struct expr *mk_int(int k);
struct expr *mk_cond(struct expr *cond, struct expr *then_br, struct expr *else_br);
