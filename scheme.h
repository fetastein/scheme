#ifndef SCHEME_H
#define SCHEME_H

#include<stdio.h>
#include<stdlib.h>
typedef int Token;

enum{
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
  Halt_Exp,
  Null_Exp
}ExpressionType;

struct Expr_t{
  int evaled;
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
Token GetToken();
Expr* NullList();
Expr* Parse();
Expr* ParseQuote();
Expr* ParseList();
Expr* ParseLiteral();
Expr* ParseSymbol();

Expr* AddList(Expr* expr);
Expr* AddSymbol(Expr* expr, char *sym);
Expr* AddNumber(Expr* expr, int i);
Expr* AddString(Expr* expr, char* str);
Expr* AddChar(Expr* expr, char ch);
Expr* AddBoolean(Expr* expr, char *booltext);

Expr* ExprTail(Expr* expr);
char* MakeSymbol(char* yytext);
void PrintExpr(Expr* expr);
void PrintValue(Expr* value);
void PrintPairValue(Expr* value);

Expr* GetSecond(Expr* expr);
Expr* GetThird(Expr* expr);

Expr* Eval(Env* env, Expr* expr, Expr* cont);
Expr* EvalQuote(Expr* expr);
Expr* EvalPair(Env* env, Expr* expr, Expr* cont);
Expr* EvalPlus(Env* env, Expr* expr, Expr* cont);
Expr* EvalMinus(Env* env, Expr* expr, Expr* cont);
Expr* EvalDefine(Env* env, Expr* expr, Expr* cont);
Expr* EvalFunction(Env* env, Expr* expr, Expr* cont);
Expr* EvalIf(Env* env, Expr* expr, Expr* cont);
Expr* EvalLambda(Env* env, Expr* expr, Expr* cont);

#endif
