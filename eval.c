#include"scheme.h"
#include"eval.h"
Expr* Eval(Env* env, Expr* expr, Expr* cont){
  if(expr->evaled == 1) {
    printf("evaled\n");
    return expr;
  }
  while(expr != NULL){

    switch(expr->type){
    case Number_Exp :

      //      puts("in Eval Number");

     return expr;
   case Symbol_Exp :
     //     puts("in Eval Symbol");
     if(strcmp(expr->u.symbol, "nil") == 0) return nil();

     Expr* res = (Expr*)lookup_expr_symbol(env, expr->u.symbol);
     //     printf("%d", res->u.int_value);
     return res;
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
  //  PrintExpr(expr);
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


Expr* EvalNullp(Env* env, Expr* expr, Expr* cont){
  Expr* ret = malloc(sizeof(Expr));
  ret->type = Bool_Exp;
  //  printf("%d", (Eval(env, GetSecond(expr), cont))->type);
  Expr* res = Eval(env, GetSecond(expr), cont);
  printf("val ");
  PrintValue(res);
  if(res == NULL){
    printf("NULL");
    ret->u.int_value = 1;
  }else{
    printf(" type %d ", res->type);
    ret->u.int_value = (res->type == Null_Exp);
    if(ret->type == Pair_Exp){
      ret->u.int_value = (res->u.list->type == Null_Exp);
    }
  }
  puts("");
  ret->next = NullList();
  return ret;
}


Expr* EvalPair(Env* env, Expr* expr, Expr* cont){
    expr = expr->u.list;
    //  expr=expr->next;
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
  }else if(strcmp(expr->u.symbol, "car") == 0){
    return EvalCar(env, expr, cont);
  }else if(strcmp(expr->u.symbol, "cdr") == 0){
    return EvalCdr(env, expr, cont);
  }else if(strcmp(expr->u.symbol, "null?") == 0){
    return EvalNullp(env, expr, cont);
  }else if(strcmp(expr->u.symbol, "pair?") == 0){
    return EvalPairp(env, expr, cont);
  }else if(strcmp(expr->u.symbol, "list")== 0){
    //    puts("to EvalList");
    Expr* ret = EvalList(env, expr, cont);
    //    puts("from EvalList");
    return ret;
  }else if(strcmp(expr->u.symbol, "let") == 0){
    puts("go let");
    return EvalLet(env, expr, cont);
  }else if(strcmp(expr->u.symbol, "cons") == 0){
    return EvalCons(env ,expr, cont);
  }else if(strcmp(expr->u.symbol, "append") == 0){
    return EvalAppend(env, expr, cont);
  }else if(strcmp(expr->u.symbol, ">") == 0){
    return EvalGT(env, expr, cont);
  }else{
    //    puts("Eval Function");
    char* funcname = expr->u.symbol;
    //    printf("funcname is %s\n", funcname);
    Expr* ret =  EvalFunction(env, expr,  cont);
    return ret;
    //    printf("funcname %s end\n", funcname);
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

  //  PrintExpr(expr);
  init_env(&new_env, env); // tmp env, it exists only while executing this function
  //  puts("DEBUG");
  //  PrintExpr(expr);
  //  puts("");



  function = Eval(env, expr, cont);

  //puts("DEBUG2");
  expr = GetSecond(expr); // get real function argument list
  args = get_expr_element(function); // get imaginary function argument list
  body = GetSecond(function); // get imaginary function body

  while(args != NULL){ // set environment
    //    puts("set arg");
    char *symbol = get_symbol_element(args);
    Expr* val_expr = Eval(env, expr, cont);
    //    int value = val_expr->u.int_value;
    record_expr(&new_env, symbol, val_expr);
    args = GetSecond(args);
    expr = GetSecond(expr);
  }
  //  Expr*  tmp =lookup_expr_symbol(&new_env, "x");
  //  printf("x is %d\n", tmp->u.int_value);
  //  printf("BODY\n");
  //    PrintExpr(body);
  //  puts("BODYend");

  return EvalPair(&new_env, body, cont);
}

/* eval if statement */

Expr* EvalIf(Env *env, Expr* expr, Expr* cont){
  Expr *test_expr, *then_expr, *else_expr;
  test_expr = expr->next;
  then_expr = expr->next->next;
  else_expr = expr->next->next->next;
  //  puts("testvalue ");
  Expr*  test_val_exp = Eval(env, test_expr, cont);
  int test_val = test_val_exp->u.int_value;

  if(test_val){
    //      printf("in true");
    Expr* ret = Eval(env, then_expr, cont);
    return ret;
  }else{
    //      printf("in false");
    return Eval(env, else_expr, cont);
  }
  fputs("Error: error in if expression\n", stderr);  
  exit(1);
}

Expr* EvalLambda(Env* env, Expr* expr, Expr* cont){
  Expr* func = GetSecond(expr);
  return func;
}
 
Expr* EvalCar(Env* env, Expr* expr,  Expr* cont){
  Expr* tmp = Eval(env, expr->next, cont);
  Expr* tmp_list = tmp->u.list;
  printf("car");
  PrintValue(tmp);
  PrintValue(tmp_list);
  puts("");
  return tmp_list;
}

Expr* EvalCdr(Env* env, Expr* expr, Expr* cont){
  Expr* tmp = Eval(env, expr->next, cont);
  printf("cdr");
  PrintValue(tmp);
  PrintValue(tmp->next);
  puts("");
  return tmp->next;
}

Expr* EvalPairp(Env* env, Expr* expr, Expr* cont){
  Expr* ret = malloc(sizeof(Expr));
  ret->type = Bool_Exp;
  ret->u.int_value = ( (Eval(env ,GetSecond(expr), cont))->type == Pair_Exp);
  ret->next = NullList();
  return ret;
}

Expr* EvalCons(Env* env, Expr* expr, Expr* cont){
  Expr* arg1_expr = expr->next;
  Expr* arg2_expr = expr->next->next;
  Expr* arg1 = Eval(env, arg1_expr, cont);
  Expr* arg2 = Eval(env, arg2_expr, cont);
  Expr* ret = malloc(sizeof(Expr));
  ret->type = Pair_Exp;
  ret->u.list = arg1;
  //  arg1->next = NullList();
  //  ar->next  = arg2;
  ret->next = arg2;
  return ret;
}
Expr* cons(Expr* arg1, Expr* arg2){

  Expr* ret = malloc(sizeof(Expr));
  ret->type = Pair_Exp;

  //  PrintExpr(arg1);

  // PrintExpr(arg2);

  ret->u.list = arg1;
  ret->next = arg2;
  ret->evaled = 1;
  return ret;
}

Expr* EvalList(Env* env, Expr* expr, Expr* cont){
  //  puts("EvalList");
return  EvalList_(env, expr->next, cont);
}

Expr* dup(Expr* expr){
  puts("dup!");
  Expr* ret = malloc(sizeof(Expr));
  ret->type = expr->type;
  ret->u = expr->u;
  ret->next = NullList();
  return ret;
}

Expr* EvalList_(Env* env, Expr* expr, Expr* cont){
  Expr* rest = expr->next;
  Expr* arg1 = dup(expr);
  //  PrintExpr(expr);
  //  puts("EvalList_");
  //  arg1->next = NullList();
  if( rest == NULL){
    return  cons(Eval(env, arg1, cont), nil());
    //    PrintExpr(ret);
    //    PrintValue(ret)

  }else{

    //    PrintExpr(rest);

    Expr* arg2 = EvalList_(env, rest, cont);

    Expr* ret =  cons(Eval(env, arg1, cont), arg2);
    ret->evaled = 1;
    return ret;
  }
}

Expr* EvalLet(Env* env, Expr* expr, Expr* cont){
  Expr* bindings = expr->next;
  Expr* bind;
  Env* new_env;
  init_env(&new_env, env);

  PrintExpr(bindings);
  while(bindings != NULL){
      printf("type is %d\n", bindings->type);
    if(bindings->type == Pair_Exp){

      bind = bindings->u.list->u.list;
      char *symbol = get_symbol_element(bind);
      printf("symbolis %s", symbol);
      printf("type is %d\n", bind->next->type);
      Expr* val_expr = Eval(env,dup( bind->next), cont);
       PrintExpr(val_expr);
       
       puts("Bindings");
       PrintExpr(bindings);
       record_expr(&new_env, symbol, val_expr);
       puts("Bindings 2");
       PrintExpr(bindings);
    }
    //    puts("aaa");
    //    printf("bindings->next is %d\n", bindings->next == NULL);
    //    printf("next bind expr's type is %d\n", bindings->type);
    puts("DEBUG");
    PrintExpr(bindings);
   bindings = GetSecond(bindings); 
   PrintExpr(bind->next->next);

  }
  puts("out let");
  return Eval(&new_env, expr->next->next, cont);
}
Expr* ListLastBefore(Expr* expr){
  if(expr->next == NULL){
    return nil();
  }else if(expr->next->type == Null_Exp){
    return expr;
  }else if(expr->next->type != Pair_Exp){
    return nil();
  }else if(expr->next->type == Pair_Exp){
    return ListLastBefore(expr->next);
  }
}


Expr* EvalAppend(Env* env, Expr* expr, Expr* cont){
  puts("DEBUG");
  Expr* arg1 = Eval(env, expr->next, cont);
  Expr* arg2 = Eval(env, expr->next->next, cont);
  puts("arg1 in append");
  PrintValue(arg1);
  puts("\narg2 in append");
  PrintValue(arg2);
  puts("");
  if(arg1->type == Null_Exp && arg2->type == Null_Exp){
    return nil();
  }else if(arg1->type == Null_Exp){
    return arg2;
  }else if(arg2->type == Null_Exp){
    return arg1;
  }else if(arg1->type == Pair_Exp && arg2->type == Pair_Exp){
    printf("list append \n");
    Expr* tail_expr = ListLastBefore(arg1);
    tail_expr->next = arg2;
    return arg1;
  }else{
    fputs("list required", stderr);
  }
}

Expr* EvalGT(Env* env, Expr* expr, Expr* cont){
  Expr* arg1  = Eval(env, expr->next, cont);
  Expr* arg2 = Eval(env, expr->next->next, cont);
  Expr* ret = malloc(sizeof(Expr));
  ret->type = Bool_Exp;

  //  printf("arg1=%d, arg2=%d\n", arg1->u.int_value, arg2->u.int_value);
  if(arg1->u.int_value > arg2->u.int_value){
    ret->u.int_value = 1;
    
        return ret;

  }else{
    //    puts("less than");
    ret->u.int_value = 0;
        return ret;

  }
}

Expr* EvalNil(Env* env, Expr* expr, Expr* cont){
  return  nil();
}

Expr* nil(){
  Expr* ret = malloc(sizeof(Expr));
  ret->type = Null_Exp;
  ret->u.int_value= 0;
  //  ret->next = NullList();
  return ret;
}
