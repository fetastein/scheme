%option noyywrap
%option array

%{
/*======line numer======*/
int mylineno = 0;

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
  Bool_Exp,
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
%}

letter 	  [\x00-\xff]
mark 	  [\!\$\%\&\*\+\-\.\/\:\<\=\>\?\@\^\_~]
alpha 	  [A-Za-z]
digit     [0-9]
ws 	  [ \t\n]
com 	  \;;
cr 	  \n
escape 	  \\
escaped   ({escape}{letter})
dquote 	  \"
ident 	  ({mark}|{alpha}|{digit})*
comment   {com}[^\n]*{cr}
string 	  {dquote}([^{escape}{dquote}]|{escaped})*{dquote}
integer   ({digit})*
boolean   \#[t|f]
if        if
quote     quote
quotemark \'
lambda    lambda
set!      set!
char      #\\{ident}
%%

{cr} mylineno++;

{ws}+        	/* skip blanks and tabs */
{comment}    	/* skip comment */

{char}          {return CHAR;}
\.            	{return DOT;}
\'              {return QUOTE_MARK;}
{integer}       {return NUMBER;}
{ident}        	{return IDENTIFIER;}
{string}    	{return STRING;}
{boolean}       {return BOOLEAN;}
\(            	{return LPAR;}
\)            	{return RPAR;}
%%

Token token = -1;

Token get_token(){
  token =  yylex();
//  printf("%s", yytext);
  return token;
}

Expr* NullList(){
  return NULL;
}

Expr* parse(){
/*parse literal or list or quote*/
  Expr* expr = NullList();
  get_token();
//  printf("%d", expr == NULL ? 1 : 0);
  switch(token){
    case QUOTE_MARK :
      expr = parse_quote();
      break;
    case LPAR :
      expr = add_list(expr);
      break;
    case STRING :
      expr = parse_literal();
      break;
    case NUMBER :
      expr = parse_literal();
      break;
    case CHAR :
      expr = parse_literal();
      break;
    case BOOLEAN : 
      expr = parse_literal();
      break;
    case RPAR :
      fputs("BAD TOKEN ')' ", stderr);
      break;
    default :
      break;
    }

  return expr;
}

char* MakeSymbol(char* yytext)
{
    char* buf = malloc(strlen(yytext) + 1);
    strcpy(buf, yytext);
    return buf;
}

Expr* parse_list(){
  Expr* expr = NullList();  
 
 while(token = get_token()){
   switch(token){
    case IDENTIFIER : 
      expr = add_symbol(expr, MakeSymbol(yytext));
      break;
    case NUMBER :
      expr = add_number(expr, atoi(yytext));
      break;
    case LPAR :
      expr = add_list(expr);
      break;
    case RPAR :
      return expr;
      break;
    default :
      break;
    }
  }
  return expr;
}

static Expr* expr_tail(Expr* expr)
{
    if(expr == NULL){
	return NULL;
    }else{
	while(expr->next != NULL)
	    expr = expr->next;
    }
    return expr;
}

Expr* add_symbol(Expr* expr, char *sym)
{
    Expr* new_expr;
    Expr* tail_expr;

    new_expr = malloc(sizeof(Expr));
    new_expr->type = Symbol_Exp;
    new_expr->u.symbol = sym;
    new_expr->next = NullList();


    tail_expr = expr_tail(expr);
    if(tail_expr == NULL){
	return new_expr;
    }else{
	tail_expr->next = new_expr;
	return expr;
    }
}


Expr* add_number(Expr* expr, int i)
{
    Expr* new_expr;
    Expr* tail_expr;

    new_expr = malloc(sizeof(Expr));
    new_expr->type = Number_Exp;
    new_expr->u.int_value = i;
    new_expr->next = NullList();

    tail_expr = expr_tail(expr);
    if(tail_expr == NULL)
	return new_expr;
    else{
	tail_expr->next = new_expr;
	return expr;
    }
}

Expr* add_list(Expr * expr){
  Expr* tail_expr;
  Expr* new_expr;

  new_expr = malloc(sizeof(Expr));
  new_expr->type = Pair_Exp;
  new_expr->u.list = parse_list();
  new_expr->next = NullList();
  
  tail_expr = expr_tail(expr);
  if(tail_expr == NULL){
      return new_expr;
  }else{
      tail_expr->next = new_expr;
      return expr;
  }
}

Expr* add_string(Expr* expr, char* str){
  return NullList();
}

Expr* add_char(Expr* expr, char ch){
  return NullList();
}

Expr* add_boolean(Expr* expr, char *booltext){
/*booltext = #\t #\f*/
  return NullList();
}

Expr* parse_quote(){
  Expr *new_expr;

  new_expr = malloc(sizeof(Expr));
  new_expr->type = Quote_Exp;
  new_expr->u.list = parse_list();
  new_expr->next = NullList();
  
  return new_expr;
}

Expr* parse_literal(){
  Expr *new_expr;

  new_expr = malloc(sizeof(Expr));
  new_expr->next = NullList();

  switch(token){
    case NUMBER :
      new_expr = add_number(NullList(), atoi(yytext));
      break;
    case STRING :
      new_expr = add_string(NullList(), MakeSymbol(yytext));
      break;
    case CHAR :
      new_expr = add_char(NullList(), yytext[0]);
      break;
    case BOOLEAN :
      new_expr = add_boolean(NullList(), yytext);
      break;
    }
}


void print_expr(Expr* expr){
   while(expr != NULL){
     switch(expr->type){
     case Number_Exp :
       printf("intvalue%d\n", expr->u.int_value);
       break;
     case Symbol_Exp :
       printf("symbolvalue%s\n", expr->u.symbol);
       break;
     case Quote_Exp :
       printf("'");
       print_expr(expr->u.list);
       break;
     case Pair_Exp :
       print_expr(expr->u.list);
       break; 
     }
    expr = expr->next;
   }
}

Expr* get_second(Expr* expr){
  Expr* second_arg = expr->next;
  return second_arg;
}

Expr* get_third(Expr* expr){
  Expr* third_arg = expr->next->next;
  return third_arg;
}

int eval_operand(Env* env, Expr* expr){
  switch(expr->type){
  case Number_Exp :
    return expr->u.int_value;
  case Symbol_Exp :
  printf("in eval_operand symbolexpression \n");
//    return lookup_number_symbol(env, expr, get_second(expr));
     return  lookup_number_symbol(env, expr->u.symbol);
  case Pair_Exp :
    return eval_Pair(env, expr);
  default :
    fputs(" Fatal Error: invalid list expression\n", stderr);
    exit(1);
  }
}

int lookup_number_symbol(Env* env, char* symbol_name){
  int i;
  printf("in lookupnumversymbol 'x'\n env->num is %d, symbol_name is %s, env->symbols[0] is %s\n ", env->num, symbol_name, env->symbols[0]);
  for(i=0; i <= env->num; i++){
    if(strcmp(env->symbols[i], symbol_name) == 0){
      if(env->kinds[i] == 0){
        return env->values[i].number;
      }else{
        fputs("lookup_number_integer(): tha value is not an integer.\n", stderr);
      }
    }
  }
  if(env->parent == NULL){
    fputs("Error: there is no such variable\n", stderr);
    exit(1);
  }else{
    return lookup_number_symbol(env->parent, symbol_name);
  }
}

Expr* lookup_expr_symbol(Env* env, char* symbol_name){
  int i;
  for(i=0; i <= env->num; i++){
    if(strcmp(env->symbols[i], symbol_name) == 0){
      if(env->kinds[i] == 1){
        return env->values[i].expr;
      }else{
        fputs("lookup_list_symbol(): tha value is not an integer.\n", stderr);
      }
    }
  }
  if(env->parent == NULL){
    fputs("Error: there is no such expr\n", stderr);
    exit(1);
  }else{
    return lookup_expr_symbol(env->parent, symbol_name);
  }
}

void record_expr(Env* env, char* symbol, Expr* expr){
  if(env->num >= ENVIRONMENT_SIZE){
    fputs("Fatal error: environment overflows.\n", stderr);
    exit(1);
  }
  int i = env->num;
  env->symbols[i] = symbol;
  env->kinds[i] = 1; // '1' means EXPR
  env->values[i].expr = expr;
  ++env->num;
}

void record_integer(Env* env, char* symbol, int value)
{
    if (env->num >= ENVIRONMENT_SIZE) {
        fputs("Fatal error: environment overflows.\n", stderr);
        exit(1);
    }
    int i = env->num;
    env->symbols[i] = symbol;
    env->kinds[i] = 0; // '0' means NUMBER
    env->values[i].number = value;
    ++env->num;
}

int eval(Env* env, Expr* expr){

  while(expr != NULL){
     printf("in numberexp\n");
    switch(expr->type){
    case Number_Exp :
//     return 0; 
     printf("aa%d\n", expr->u.int_value);
     return expr->u.int_value;
   case Quote_Exp :
     eval_Quote(env, expr);     
     return 0;
   case Pair_Exp :
     return eval_Pair(env, expr);
   }
 expr = expr->next;
 }
}

int eval_Quote(Expr* expr){
  print_expr(expr);
  return 0;
}

int eval_Pair(Env* env, Expr* expr){
  expr = expr->u.list;
  if(strcmp(expr->u.symbol, "+") == 0){
    printf("in plus\n");
    return printf("%d", eval_plus(env, expr));
  }else if(strcmp(expr->u.symbol, "-") == 0){
    return eval_minus(env, expr);
  }else if(strcmp(expr->u.symbol, "define") == 0){
    return eval_define(env, expr);
  }else if(strcmp(expr->u.symbol, "defun") == 0){
    printf("in defun\n");
    return eval_defun(env, expr);
  }else if(strcmp(expr->u.symbol, "if") == 0){
  printf("in if%s\n", expr->u.symbol);
    return eval_if(env, expr);
  }else{
    printf("in function\n");
    return eval_function(env, expr, expr->u.symbol);
  }
}

int eval_plus(Env* env, Expr* expr){
//  printf("%d", expr->type == Pair_Exp ? 123 : 321);
  int x ,y; 
  printf("in plus, expr's ymbol is %s\n expr->next->u.symbol is %s\n", expr->u.symbol, get_second(expr)->u.symbol);
  x = eval_operand(env, get_second(expr));
  printf("after eval first operand in (+ x 2)");
  y = eval_operand(env, get_third(expr));
  return x + y;
}

int eval_minus(Env* env, Expr* expr){
  return eval_operand(env, get_second(expr)) - eval_operand(env, get_third(expr));
}

/* DEFINE */ 
int eval_define(Env* env, Expr* expr){
  Expr* symbol_expr = get_second(expr);
  char* symbol = symbol_expr->u.symbol;

  Expr* value_expr = get_third(expr);
  int value = value_expr->u.int_value;
  record_integer(env, symbol, value);
 return value;
}

/* DEFUN */
int eval_defun(Env* env, Expr* expr){
  Expr* symbol_expr = get_second(expr);
  char* symbol = symbol_expr->u.symbol;
  printf("function symbol is %s\n", symbol);  

  Expr* func_body = get_third(expr);
//  Expr* func_body = func_body_expr->u.list;
  printf("function body is <\n");
  print_expr(func_body);
  printf(">\n");
  record_expr(env, symbol, func_body);
  return 0;
}

/* element list */
Expr *get_expr_element(Expr *expr){
  return expr->u.list;
}

/*element symbol */
char *get_symbol_element(Expr *expr){
  return expr->u.symbol;
}

/* init environmet */
void init_env(Env* env, Env* parent){
  env->num = 0;
  env->parent = parent;
}

/* eval function */

int eval_function(Env* env, Expr* expr, char * op){
  Expr *args, *body, *function;
  Env new_env;
 init_env(&new_env, env); // tmp env, it exists only while executing this function

  expr = get_second(expr); // get real function argument list
  printf("env init after\n");
  function = lookup_expr_symbol(env, op);// get imaginary function
  print_expr(function);
  args = get_expr_element(function); // get imaginary function argument list
  print_expr(args);
  body = get_second(function); // get imaginary function body 
  print_expr(body);

    printf("body's symbol is %s\n env->num is %d \n", body->u.list->u.symbol, env->num);

  while(args != NULL){ // set environment
    char *symbol = get_symbol_element(args);
    int value = eval_operand(env, expr);
    printf("value is %d; symbol is %s; new_env->num is %d\n", value, symbol, (&new_env)->num);
    record_integer(&new_env, symbol, value);

    args = get_second(args);
    printf("%s\n", args == NULL ? "args is NULL!" : "args is NOT NULL!");
    expr = get_second(expr);
  }
  printf("here\n");
  return printf("<%d>", eval_Pair(&new_env, body));
}

/* eval if statement */

int eval_if(Env *env, Expr* expr){
  Expr *test_expr, *then_expr, *else_expr;
  test_expr = expr->next;
  then_expr = expr->next->next;
  else_expr = expr->next->next->next;
  
  if(eval(env, test_expr)){
  printf("in true");
    return eval(env, then_expr);
  }else{
  printf("in false");
    return eval(env, else_expr);
  }
  fputs("Error: error in if expression\n", stderr);  
  exit(1);
}

int main(void)
{
Expr* expr;
int x;
Env env;
 init_env(&env, NULL);
for(;;){
expr = parse();
print_expr(expr);
if(expr == NULL){
  break;
}else{
  eval(&env, expr);
}
}
return 0;
}


