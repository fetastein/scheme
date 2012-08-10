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
  printf("%s", yytext);
  return token;
}

Expr* NullList(){
  return NULL;
}

Expr* parse(){
/*parse literal or list or quote*/
  Expr* expr = NullList();
 printf("AA");
  get_token();
  printf("%d", expr == NULL ? 1 : 0);
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
    printf("add_symbol%s", sym); // printf("%s", yytext);
 
    Expr* new_expr;
    Expr* tail_expr;

    new_expr = malloc(sizeof(Expr));
    new_expr->type = Symbol_Exp;
    new_expr->u.symbol = sym;
    new_expr->next = NullList();


    tail_expr = expr_tail(expr);
    if(tail_expr == NULL){
	printf("<%s>", new_expr->u.symbol);
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
 printf("<inparsequote>");

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
       printf("intvalue%d", expr->u.int_value);
       break;
     case Symbol_Exp :
       printf("symbolvalue%s", expr->u.symbol);
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
    return lookup_number_symbol(env, expr, get_second(expr));
  case Pair_Exp :
    return eval_Pair(env, expr);
  default :
    fputs(" Fatal Error: invalid list expression\n", stderr);
    exit(1);
  }
}

int lookup_number_symbol(Env* env, char* symbol_name){
  int i;
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
    switch(expr->type){
    case Number_Exp :
     printf("%d", expr->u.int_value);
     return 0;
/*   case Symbol_Exp :
     printf("%s", expr->u.symbol);
     return 0;*/
   case Quote_Exp :
     eval_Quote(env, expr);     
     return 0;
   case Pair_Exp :
     printf("HEW");
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
  printf("which_expr?%s\n", expr->u.symbol);
  expr = expr->u.list;
  printf("it's symbol is %d", expr==NULL ? 100 :  111);
  if(strcmp(expr->u.symbol, "+") == 0){
//    return eval_plus(env, expr);
    printf("value is %d", eval_plus(env, expr));

    return 0;
  }else if(strcmp(expr->u.symbol, "-") == 0){
    return printf("%d", eval_minus(env, expr));
  }else if(strcmp(expr->u.symbol, "define") == 0){
    return eval_define(env, expr);
  }
}
int eval_plus(Env* env, Expr* expr){
  printf("%d", expr->type == Pair_Exp ? 123 : 321);
  return eval_operand(env, get_second(expr)) + eval_operand(env, get_third(expr));
}

int eval_minus(Env* env, Expr* expr){
  return eval_operand(env, get_second(expr)) - eval_operand(env, get_third(expr));
}

int eval_define(Env* env, Expr* expr){
  Expr* symbol_expr = get_second(expr);
  char* symbol = symbol_expr->u.symbol;

  Expr* value_expr = get_third(expr);
int value = value_expr->u.int_value;
  printf("here?");
  record_integer(env, symbol, value);
 return value;
}

Env* init_env(Env *env){
  env = malloc(sizeof(Env));
  env->num = 0;
  return env;
}

int main(void)
{
Expr* expr;
int x;
Env* env = init_env(env);
for(;;){
expr = parse();
if(expr == NULL){
  break;
}else{
  eval(env, expr);
}
}
return 0;
}


