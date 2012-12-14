#include "scheme.h"
#ifndef CONV_H
#define CONV_H

extern int lambda_number;


Expr* Conv(Env* env, Expr* expr, Expr* cont);
Expr* ConvNumber(Env* env, Expr* expr, Expr* cont);
Expr* ConvPair(Env* env, Expr* expr, Expr* cont);
Expr* ConvDefine(Env* env, Expr* expr, Expr* cont);
Expr* ConvBegin(Env* env, Expr* expr, Expr* cont);
#endif
