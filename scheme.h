
/*======token enumelator======*/
typedef int Token;

enum {
  DOT,
  IDENTIFIER,
  STRING,
  BOOLEAN,
  LPAR,
  RPAR,
  SHARP,
  QUOTE_MARK,
  CHAR,
  NUMBER
};

/*======expression definition======*/
typedef enum{
  Bool_Exp=0,
  Number_Exp,
  Str_Exp,
  Char_Exp,
  Quote_Exp,
  Symbol_Exp,
  Pair_Exp,
}ExpressionType;

struct Expr_t{
  ExpressionType type;
  union{
    int int_value;
    char *symbol;
    struct Expr_t *list;
  }u;
  struct Expr_t *next;
};

typedef struct Expr_t    Expr;

/*======Environment struct definition======*/
#define ENVIRONMENT_SIZE 64

struct Env_t {
    int         num;
    char*       symbols[ENVIRONMENT_SIZE];
    int         kinds[ENVIRONMENT_SIZE];   /* '0' means NUMBER or '1' means LIST */
    union {
        int     number;
        Expr*   expr;
    } values[ENVIRONMENT_SIZE];
    struct Env_t* parent;
};

typedef struct Env_t Env;


Expr* parse_list();
Expr* parse_literal();
Expr* add_symbol(Expr* expr, char *sym);
Expr* add_number(Expr* expr, int i);
Expr* add_list(Expr* expr);
Expr* parse_quote();
Expr* add_string(Expr* expr, char *str);
Expr* add_char(Expr* expr, char ch);
Expr* add_boolean(Expr* expr, char *booltext);

Expr* eval_operand(Env* env, Expr* expr, Expr* cont);
Expr* eval(Env* env, Expr* expr,Expr* cont);
Expr* eval_Quote(Expr* expr);
Expr* eval_Pair(Env* env, Expr* expr, Expr* cont);
Expr* eval_plus(Env* env, Expr* expr, Expr* cont);
Expr* eval_minus(Env* env, Expr* expr, Expr* cont);
Expr* eval_define(Env* env, Expr* expr, Expr* cont);
Expr* eval_function(Env* env, Expr* expr, char* op, Expr* cont);
Expr* eval_if(Env* env, Expr* expr, Expr* cont);



#define LAMBDA_NAME_LENGTH 100

