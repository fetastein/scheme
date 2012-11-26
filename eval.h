#ifndef EVAL_H
#define EVAL_H
#include"scheme.h"
Expr* Eval(Env* env, Expr* expr, Expr* cont);
Expr* EvalQuote(Expr* expr);
Expr* EvalPair(Env* env, Expr* expr, Expr* cont);
Expr* EvalPlus(Env* env, Expr* expr, Expr* cont);
Expr* EvalMinus(Env* env, Expr* expr, Expr* cont);
Expr* EvalMul(Env* env, Expr* expr, Expr* cont);
Expr* EvalDefine(Env* env, Expr* expr, Expr* cont);
Expr* EvalFunction(Env* env, Expr* expr, Expr* cont);
Expr* EvalIf(Env* env, Expr* expr, Expr* cont);
Expr* EvalLambda(Env* env, Expr* expr, Expr* cont);

Expr* get_expr_element(Expr* expr);
char* get_symbol_element(Expr* expr);


#endif
