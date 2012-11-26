#include"scheme.h"
#include"eval.h"
Expr* Eval(Env* env, Expr* expr, Expr* cont){

  while(expr != NULL){

    switch(expr->type){
    case Number_Exp :



     return expr;
   case Symbol_Exp :
     return lookup_expr_symbol(env, expr->u.symbol);
   case Quote_Exp :
     return (Expr *)EvalQuote(expr);     

     return NullList();
   case Pair_Exp :

     return EvalPair(env, expr, cont);
   }
 expr = expr->next;
 }
}

Expr* EvalQuote(Expr* expr){
  PrintExpr(expr);
  return NullList();
}


Expr* EvalMul(Env* env, Expr* expr, Expr* cont){
  int x ,y; 
  Expr *x_expr, *y_expr;
  x_expr = Eval(env, GetSecond(expr), cont);
  x = x_expr->u.int_value;
  y_expr = Eval(env, GetThird(expr), cont);
  y = y_expr->u.int_value;
  printf("%d\n", x * y);
  Expr *ret_val = malloc(sizeof(Expr));
  ret_val->type = Number_Exp;
  ret_val->u.int_value = x * y;
  return ret_val;
}


Expr* EvalPair(Env* env, Expr* expr, Expr* cont){
  expr = expr->u.list;
  if(strcmp(expr->u.symbol, "+") == 0){
    return EvalPlus(env, expr, cont);
  }else if(strcmp(expr->u.symbol, "-") == 0){
    return EvalMinus(env, expr, cont);
  }else if(strcmp(expr->u.symbol, "*") == 0){
    return EvalMul(env, expr, cont);
  }else if(strcmp(expr->u.symbol, "define") == 0){
    return EvalDefine(env, expr, cont);
  }else if(strcmp(expr->u.symbol, "if") == 0){
    return EvalIf(env, expr, cont); 
  }else if(strcmp(expr->u.symbol, "lambda") == 0){
    return EvalLambda(env, expr, cont);
  }else if(strcmp(expr->u.symbol, "halt") == 0){
    return expr; // return Halt expression.
  }else{
    puts("Eval Function");
    return EvalFunction(env, expr,  cont);
  }
}

Expr* EvalPlus(Env* env, Expr* expr, Expr* cont){
  int x ,y; 
  Expr *x_expr, *y_expr;
  x_expr = Eval(env, GetSecond(expr), cont);
  x = x_expr->u.int_value;
  y_expr = Eval(env, GetThird(expr), cont);
  y = y_expr->u.int_value;
  printf("%d\n", x + y);
  Expr *ret_val = malloc(sizeof(Expr));
  ret_val->type = Number_Exp;
  ret_val->u.int_value = x + y;
  return ret_val;
}


Expr* EvalMinus(Env* env, Expr* expr, Expr* cont){
  int x ,y; 
  Expr *x_expr, *y_expr;
  x_expr = Eval(env, GetSecond(expr), cont);
  x = x_expr->u.int_value;
  y_expr = Eval(env, GetThird(expr), cont);
  y = y_expr->u.int_value;
  printf("%d\n", x- y);
  Expr *ret_val = malloc(sizeof(Expr));
  ret_val->type = Number_Exp;
  ret_val->u.int_value = x - y;
  return ret_val;
}

/* DEFINE */ 
Expr* EvalDefine(Env* env, Expr* expr, Expr* cont){
  Expr* symbol_expr = GetSecond(expr);
  char* symbol = symbol_expr->u.symbol;

  //  Expr* value_expr = get_third(expr);
  Expr* value_expr = Eval(env, GetThird(expr), cont);
//  int value = value_expr->u.int_value;
  Expr* value = value_expr;
  record_expr(env, symbol, value);
  return value;

}

/* element list */
Expr *get_expr_element(Expr *expr){
  return expr->u.list;
}

/*element symbol */
char *get_symbol_element(Expr *expr){
  return expr->u.symbol;
}

/* eval function */

Expr* EvalFunction(Env* env, Expr* expr, Expr* cont){
  Expr *args, *body, *function;
  Env new_env;
  PrintExpr(expr);
  init_env(&new_env, env); // tmp env, it exists only while executing this function
  puts("DEBUG");
  PrintExpr(expr);
  puts("");



  function = Eval(env, expr, cont);

puts("DEBUG2");  expr = GetSecond(expr); // get real function argument list
  args = get_expr_element(function); // get imaginary function argument list
  body = GetSecond(function); // get imaginary function body

  while(args != NULL){ // set environment
    char *symbol = get_symbol_element(args);
    Expr* val_expr = Eval(env, expr, cont);
    int value = val_expr->u.int_value;
    record_expr(&new_env, symbol, val_expr);
    args = GetSecond(args);
    expr = GetSecond(expr);
  }
  //  Expr*  tmp =lookup_expr_symbol(&new_env, "x");
  //  printf("x is %d\n", tmp->u.int_value);
  //  printf("BODY\n");
    PrintExpr(body);
  //  puts("BODYend");
  return EvalPair(&new_env, body, cont);
}

/* eval if statement */

Expr* EvalIf(Env *env, Expr* expr, Expr* cont){
  Expr *test_expr, *then_expr, *else_expr;
  test_expr = expr->next;
  then_expr = expr->next->next;
  else_expr = expr->next->next->next;
  
  Expr*  test_val_exp = Eval(env, test_expr, cont);
  int test_val = test_val_exp->u.int_value;

  if(test_val){
  printf("in true");
    return Eval(env, then_expr, cont);
  }else{
  printf("in false");
    return Eval(env, else_expr, cont);
  }
  fputs("Error: error in if expression\n", stderr);  
  exit(1);
}

Expr* EvalLambda(Env* env, Expr* expr, Expr* cont){
  Expr* func = GetSecond(expr);
  return func;
}
