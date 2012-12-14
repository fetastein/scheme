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
Expr* EvalCar(Env* env, Expr* expr, Expr* cont);
Expr* EvalCdr(Env* env, Expr* expr, Expr* cont);
Expr* EvalNullp(Env* env, Expr* expr, Expr* cont);
Expr* EvalPairp(Env* env, Expr* expr, Expr* cont);
Expr* EvalList(Env* env, Expr* expr, Expr* cont);
Expr* EvalList_(Env* env, Expr* expr, Expr*  cont);
Expr* EvalLet(Env* env, Expr* expr, Expr* cont);
Expr* EvalCons(Env* env, Expr* expr, Expr* cont);
Expr* EvalAppend(Env* env, Expr* expr, Expr* cont);
Expr* EvalGT(Env* env, Expr* expr, Expr* cont);
Expr* nil();


Expr* get_expr_element(Expr* expr);
char* get_symbol_element(Expr* expr);


#endif
