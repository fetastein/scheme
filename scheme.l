%option noyywrap
%option array

%{

  /*======line numer======*/
  int mylineno = 0;

#include "scheme.h"
#include "eval.h"
#include "conv.h"
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

Token  token = -1;

Token GetToken(){
  token = yylex();
  return token;
}

Expr* NullList(){
  return (Expr*)NULL;
}

Expr* Parse(){
  GetToken();
  Expr* expr = NullList();
  switch(token){
  case QUOTE_MARK:
    expr = ParseQuote();
    break;
  case LPAR:
    expr = AddList(expr);
    break;
  case STRING:
    expr = ParseLiteral();
    break;
  case NUMBER:
    expr = ParseLiteral();
    break;
  case CHAR:
    expr = ParseLiteral();
    break;
  case IDENTIFIER :
    expr = ParseSymbol();
    PrintExpr(expr);
    break;
  case RPAR:
    fputs("BAD TOKEN ')'", stderr);
    break;
  default:
    break;
  }
  return expr;
}

Expr* ParseSymbol(){
  Expr* expr = NullList();
  expr = AddSymbol(expr, MakeSymbol(yytext));
  return expr;
}

char* MakeSymbol(char* yytext)
{
    char* buf = malloc(strlen(yytext) + 1);
    strcpy(buf, yytext);
    return buf;
}

Expr* ParseList(){
  Expr* expr = NullList();  

  while(token = GetToken()){
   switch(token){
   case IDENTIFIER : 
     expr = AddSymbol(expr, MakeSymbol(yytext));
     break;
   case NUMBER :
     expr = AddNumber(expr, atoi(yytext));
      break;
   case LPAR :
     expr = AddList(expr);
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

Expr* ExprTail(Expr* expr)
{
  if(expr == NULL){
    return NULL;
  }else{
    while(expr->next != NULL)
      expr = expr->next;
  }
  return expr;
}

Expr* AddSymbol(Expr* expr, char *sym)
{
  Expr* new_expr;
  Expr* tail_expr;

  new_expr = malloc(sizeof(Expr));
  new_expr->type = Symbol_Exp;
  new_expr->u.symbol = sym;
  new_expr->next = NullList();

  
  tail_expr = ExprTail(expr);
  if(tail_expr == NULL){
    return new_expr;
  }else{
    tail_expr->next = new_expr;
    return expr;
  }
}


Expr* AddNumber(Expr* expr, int i)
{
  Expr* new_expr;
  Expr* tail_expr;
  
  new_expr = malloc(sizeof(Expr));
  new_expr->type = Number_Exp;
  new_expr->u.int_value = i;
  new_expr->next = NullList();
  
  tail_expr = ExprTail(expr);
  if(tail_expr == NULL)
    return new_expr;
  else{
    tail_expr->next = new_expr;
    return expr;
  }
}

Expr* AddList(Expr * expr){
  Expr* tail_expr;
  Expr* new_expr;
  
  new_expr = malloc(sizeof(Expr));
  new_expr->type = Pair_Exp;
  new_expr->u.list = ParseList();
  new_expr->next = NullList();
  
  tail_expr = ExprTail(expr);
  if(tail_expr == NULL){
    return new_expr;
  }else{
    tail_expr->next = new_expr;
    return expr;
  }
}

Expr* AddString(Expr* expr, char* str){
  return NullList();
}

Expr* AddChar(Expr* expr, char ch){
  return NullList();
}

Expr* AddBoolean(Expr* expr, char *booltext){
  /*booltext = #\t #\f*/
  return NullList();
}

Expr* ParseQuote(){
  Expr *new_expr;
  
  new_expr = malloc(sizeof(Expr));
  new_expr->type = Quote_Exp;
  new_expr->u.list = ParseList();
  new_expr->next = NullList();
  
  return new_expr;
}

Expr* ParseLiteral(){
  Expr *new_expr;
  
  new_expr = malloc(sizeof(Expr));
  new_expr->next = NullList();
  
  switch(token){
  case NUMBER :
    new_expr = AddNumber(NullList(), atoi(yytext));
    break;
  case STRING :

    break;
  case CHAR :
    new_expr = AddChar(NullList(), yytext[0]);
    break;
  case BOOLEAN :
    new_expr = AddBoolean(NullList(), yytext);
    break;
  }
  return new_expr;
}


int depth = 0;
void PrintExpr(Expr* expr){
  while(expr != NULL){
      int buf = depth;
    switch(expr->type){
    case Number_Exp :
      printf("int value: %d\n", expr->u.int_value);
      break;
    case Symbol_Exp :
      printf("symbol value :%s\n", expr->u.symbol);
      break;
    case Quote_Exp :
      printf("quote");
      PrintExpr(expr->u.list);
      break;
    case Pair_Exp :
      printf("( LPAREN level: %d START\n", ++depth);
      PrintExpr(expr->u.list);
      printf(") RPAREN level :%d END\n", depth--);
      break; 
    case Halt_Exp :
      printf("HALT\n");
    case Bool_Exp :
      printf("bool valueis %d\n", expr->u.int_value);
      break;
    case Null_Exp:
      printf("NULL expr\n");
      break;
    default :
    if(expr == NULL) puts("NULL!");
      break;
    }

    expr = expr->next;
  }

}


Expr* GetSecond(Expr* expr){
  Expr* second_arg = expr->next;
  return second_arg;
}

Expr* GetThird(Expr* expr){
  Expr* third_arg = expr->next->next;
  return third_arg;
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

Expr *lookup_expr_symbol(Env* env, char* symbol_name){
  int i;
  //  printf("in lookup '%s' expr symbol\n", symbol_name);
  for(i=0; i < env->num; i++){
    if(strcmp(env->symbols[i], symbol_name) == 0){
      return env->values[i].expr;
    }else{
      //      fputs("lookup_list_symbol(): tha value is not an integer.\n", stderr);
    }
  }

//    printf("Not found %s in current Env%s", symbol_name);
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
  //  printf("////%s in env [%d]===", symbol, i);
  //  PrintValue(expr);
  //  printf("===%s", symbol);
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

/* init environmet */
void init_env(Env* env, Env* parent){
  env->num = 0;
  env->parent = parent;
}
// CONT endpoint
Expr* cont_endpoint;
void PrintValue(Expr* value){
  while(value != NULL){
    switch(value->type){
    case Number_Exp :
      printf("%d", value->u.int_value);
      //      break;
      return;
    case Symbol_Exp :
      printf("%s", value->u.symbol);
      break;
    case Quote_Exp :
      printf("quote");
      PrintPairValue(value);
      return;
    case Pair_Exp :
      PrintPairValue(value);
      return;
    case Halt_Exp :
      printf("HALT\n");
    case Bool_Exp :
      printf("%s", value->u.int_value ? "#t" : "#f");
      break;
    case Null_Exp :
      printf("nil");
      break;
    default :
      break;
    }
      value = value->next;
  }

}
void PrintPairValue(Expr* value){
    printf("(");
    PrintValue_(value);
    printf(")");
}

void PrintValue_(Expr* value){
  PrintValue(value->u.list);
  if(value->next == NULL){
    printf("next null");
    return;
  }
  if(value->next->type == Pair_Exp){
    printf(" ");
    PrintValue_(value->next);
  }else if(value->next->type == Null_Exp){
    return;
  }else{
    printf(" . ");
    PrintValue(value->next);
    return;
  }
}

int main(int argc, char** argv){

  ++argv, --argc;
  if(argc >0){
    yyin = fopen(argv[0], "r");
  }else{
    yyin = stdin;
  }
  Env* env = malloc(sizeof(Env));
  int lambda_number = 0;
  init_env(env, NULL);
  Expr* halt = malloc(sizeof(Expr));
  Expr* cont = malloc(sizeof(Expr));
  Expr* lambda= malloc(sizeof(Expr));
  Expr* args= malloc(sizeof(Expr));
  Expr* arg = malloc(sizeof(Expr));
  Expr* body = malloc(sizeof(Expr));
  cont->type = Pair_Exp;
  cont->u.list = lambda;
  cont->next = NullList();
  lambda->type = Symbol_Exp;
  lambda->u.symbol = "lambda";
  lambda->next = args;
  args->type = Pair_Exp;
  args->u.list = arg;
  args->next = body;
  body->type = Pair_Exp;
  body->u.list = halt;
  halt->type = Halt_Exp;
  halt->next = arg;
  halt->u.symbol = "halt";  
  // halt->nextに処理したい対象を入れる。
  arg->type = Symbol_Exp;
  char arg_name[100];
  sprintf(arg_name, "r_%d", lambda_number++); //lambda argument
  arg->u.symbol = arg_name;
  arg->next = NullList(); 

  cont_endpoint =halt->next;
  
  for(;;){
    printf(">");
    Expr* expr = Parse();
    if(expr == NULL){
      puts("END OF PROMPT");
      break;
    }else{
            PrintExpr(expr);
/*            puts("START conv");
            expr = Conv(&env, expr, cont);
            puts("---Conversion...\n===Return AST===");
            PrintExpr(expr);
            puts("END"); */
      puts("START eval");
      expr = Eval(&env, expr, NULL);
      puts("---Eval...");
      depth = 0;
      puts("===Return Value===");
      PrintValue(expr);
      puts("END");
/*            if(expr->type == Halt_Exp){
      	puts("HALT!");
           goto END;
         }*/
    }
  }
 END:
  return 0;
}
