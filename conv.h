#include "scheme.h"
#ifndef CONV_H
#define CONV_H

// primitives, primitives number and max length 
#define PRI_NUM 6
#define PRI_MAXLEN 10



Expr* Conv(Env* env, Expr* expr, Expr* cont);
Expr* ConvLiteral(Env* env, Expr* expr, Expr* cont);
Expr* ConvPair(Env* env, Expr* expr, Expr* cont);
Expr* ConvDefine(Env* env, Expr* expr, Expr* cont);
Expr* ConvBegin(Env* env, Expr* expr, Expr* cont);
Expr* ConvApply(Env* env, Expr* expr, Expr* cont);
Expr* ConvPrimitive(Env* env, Expr* expr, Expr* cont);
#endif
