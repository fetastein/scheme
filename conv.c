#include"scheme.h"
#include"conv.h"
Expr* cont_endpoint;
int lambda_number = 1;



Expr* duprec(Expr* expr){
  Expr* ret = malloc(sizeof(Expr));

  ret->type = expr->type;
  if(expr->type == Pair_Exp){
    ret->u.list = duprec(expr->u.list);
  }else{
    ret->u = expr->u;
  }
  if(expr->next == NULL){
    ret->next = NullList();
  }else if(expr->next->type == Pair_Exp){
    ret->next = duprec(expr->next);
  }else{
    ret->next = expr->next;
  }
  return ret;
}


Expr* ConvLiteral(Env* env, Expr* expr, Expr* cont){
  Expr* ret = malloc(sizeof(Expr));
  ret->type = Pair_Exp;
  ret->u.list = cont;
  cont->next = expr;
  return ret;
}

Expr* ConvNumber(Env* env, Expr* expr, Expr* cont){
  return expr;
}
char* makearg_name(){
  char* arg_name = malloc(sizeof(char) * 64);
  sprintf(arg_name, "r_%d", lambda_number++); //lambda argument
  return arg_name;
}

Expr* makearg(char* arg_name){
  Expr* ret = malloc(sizeof(Expr));
  ret->type = Symbol_Exp;
  ret->u.symbol = arg_name;
  ret->next = NullList();
  return ret;
}

Expr* makelambda(Expr* img_args, Expr* val){
  Expr* ret, *lambda, *args;
  ret = malloc(sizeof(Expr));
  lambda = malloc(sizeof(Expr));
  args = malloc(sizeof(Expr));

  ret->type = Pair_Exp;
  ret->u.list = lambda;
  ret->next = NullList();
  
  lambda->type = Symbol_Exp;
  lambda->u.symbol = "lambda";
  lambda->next = args;
  
  args->type = Pair_Exp;
  args->u.list = img_args;
  args->next = val;
  
  return ret;
}
//img = imaginary

Expr* ConvIf(Env* env, Expr* expr, Expr* cont){
  puts("in Conv if");
  Expr* e1 = GetSecond(expr);
  Expr* e2 = GetThird(expr);
  Expr* e3 = e2->next; 
  puts("   e2");
  PrintExpr(e2);
  puts("   e3");
  PrintExpr(e3);
  printf("    dup(e2)");
  PrintExpr(dup(e2));
  e2 = Conv(env, dup(e2), duprec(cont));
  puts("   convede2");
  PrintExpr(e2);
  Expr* duprece2 = duprec(e2);
  puts("   duprece2");
  PrintExpr(duprece2);
  e3 = Conv(env, dup(e3), cont);
  puts("e3");
  PrintExpr(e3);

  Expr* new_cont, *r1, *ifexp, *val;
  new_cont = malloc(sizeof(Expr));
  r1 = malloc(sizeof(Expr));
  ifexp = malloc(sizeof(Expr));
  val = malloc(sizeof(Expr));

  val->type = Pair_Exp;
  val->u.list = ifexp;
  val->next = NullList();
  
  r1 = makearg(makearg_name());
  
  ifexp->type = Symbol_Exp;
  ifexp->u.symbol = "if";
  ifexp->next = r1;
  
  r1->next = e2;
  e2->next = e3;
  puts("e2");
  PrintExpr(e2);
  puts("e3");
  PrintExpr(e3);

  new_cont = makelambda(dup(r1), val);

  return Conv(env, e1, new_cont);
}

char primitives[PRI_NUM][PRI_MAXLEN] = {"+", "-", "*", "cdr", ">", "<"};
Expr* ConvApplyDispatch(Env* env, Expr* expr, Expr* cont){
  int primitivep = 0, i;
  //  printf("%s is operator; %s is primitves[0]\n", expr->u.symbol, primitives[0]);
  for(i = 0; i < PRI_NUM; i++){
        printf("%s\n", primitives[i]);
    if(strcmp(primitives[i], expr->u.symbol)==0) primitivep = 1;
    //    printf("aa\n");
  }
  puts("DEBUG");
  if(primitivep == 1){
    printf("to conv primitive\n");
    return ConvPrimitive(env, expr, cont);
  }else{
    printf("to conv apply");
    return ConvApply(env, expr, cont);
  }
}
    

Expr* ConvPrimitive(Env* env, Expr* expr, Expr* cont){
  Expr *real_args, *lstargcont, *lstargcontlambda, *lstargcontargs, *operator, *convedargs, *operation, *val;
  char ** arg_namearr = malloc(sizeof(char[16]) * 32);
  Expr** valarr = malloc(sizeof(Expr) * 32);
  Expr** argarr = malloc(sizeof(Expr) * 32);
  int p = 0, i;  
  lstargcont = malloc(sizeof(Expr));
  lstargcontlambda = malloc(sizeof(Expr));
  lstargcontargs = malloc(sizeof(Expr));
  operation = malloc(sizeof(Expr));
  val = malloc(sizeof(Expr));

  
  operator = dup(expr);
  real_args = expr->next; //except operator 
  while(real_args){
    arg_namearr[p] = makearg_name();
    valarr[p] = real_args;
    p++;
    real_args = real_args->next;
  }
  argarr[p] = makearg(arg_namearr[p]);
  for(i = p-1; i >= 0; i--){
    argarr[i] = makearg(arg_namearr[i]); 
    argarr[i]->next = argarr[i+1];
  }

  lstargcont->type = Pair_Exp;
  lstargcont->u.list = lstargcontlambda;
  lstargcont->next = NullList();
  
  lstargcontlambda->type = Symbol_Exp;
  lstargcontlambda->u.symbol = "lambda";
  lstargcontlambda->next = lstargcontargs;
 
  lstargcontargs->type = Pair_Exp;
  lstargcontargs->u.list = dup(argarr[p-1]);
  lstargcontargs->next = operation;
  
  operation->type = Pair_Exp;
  operation->u.list = cont;
  operation->next = NullList();
  
  cont->next = val;
  
  val->type = Pair_Exp;
  val->u.list = operator;
  val->next = NullList();
  
  operator->next = argarr[0];

  //  puts("OPERATOR");
  //  PrintExpr(lstargcont);
  //  puts("END");
  for(i = p-1; i > 0; i--){
    //    puts("VAL");
    //    PrintExpr(valarr[i]);

    Expr* tmp = Conv(env, valarr[i], lstargcont);
    lstargcont = makelambda(dup(argarr[i-1]), tmp);
    //    printf("CONV tmp i=%dn", i);
    //    PrintExpr(tmp);
    //    puts("END");
    //    puts("CONV lstargcont");
    //    PrintExpr(lstargcont);
    //    puts("END");
  }
  convedargs = Conv(env, valarr[0], lstargcont);
  return convedargs;
}
  

Expr* ConvApply(Env* env, Expr* expr, Expr* cont){
  Expr *real_args, *lstargcont, *lstargcontlambda, *lstargcontargs, *arg0, *convedval, *val;
  char ** arg_namearr = malloc(sizeof(char[16]) * 32);
  Expr** valarr = malloc(sizeof(Expr) * 32);
  Expr** argarr = malloc(sizeof(Expr) * 32);
  int p = 0, i;  
  lstargcont = malloc(sizeof(Expr));
  lstargcontlambda = malloc(sizeof(Expr));
  lstargcontargs = malloc(sizeof(Expr));
  val = malloc(sizeof(Expr));

  real_args = expr;
  //  puts("arg0");
  //  PrintExpr(real_args);
  //  puts("arg1");
  //  PrintExpr(real_args->next);
  while(real_args){
    arg_namearr[p] = makearg_name();
    valarr[p] = real_args;
    p++;
    real_args = real_args->next;
  }
  //  puts("DEBUG");
  argarr[p] = makearg(arg_namearr[p]);
  for(i = p-1; i >= 0; i--){
    argarr[i] = makearg(arg_namearr[i]); 
    argarr[i]->next = argarr[i+1];
  }

  lstargcont->type = Pair_Exp;
  lstargcont->u.list = lstargcontlambda;
  lstargcont->next = NullList();
  
  lstargcontlambda->type = Symbol_Exp;
  lstargcontlambda->u.symbol = "lambda";
  lstargcontlambda->next = lstargcontargs;
 
  printf("===argarr[p-1]");
  PrintExpr(argarr[p-1]);

  arg0 = dup(argarr[0]);

  lstargcontargs->type = Pair_Exp;
  lstargcontargs->u.list = dup(argarr[p-1]);
  lstargcontargs->next = val;//arg0;
  
  val->type = Pair_Exp;
  val->u.list = arg0;
  val->next = NullList();

  //  puts("ARG0");
  //  PrintExpr(arg0);
  arg0->next = cont;
  cont->next = (argarr[1] == NULL) ? NULL: argarr[1];
  //  puts("ARGARR[1]");
  //  PrintExpr(argarr[1]);
  //  puts("lstargcont");
  //  PrintExpr(lstargcont);
  convedval = Conv(env, valarr[p-1], lstargcont);
  for(i = p-1; i >= 0; i--){
    printf("lstargcont p=%d", i);
    PrintExpr(lstargcont);  
    lstargcont = Conv(env, valarr[i], lstargcont);
    printf("CONVEDlstargcont p=%d", i);
    PrintExpr(lstargcont);
    if(i>0) lstargcont = makelambda(dup(argarr[i-1]), lstargcont);
  }


  return lstargcont;
}
  
Expr* ConvBegin(Env* env, Expr* expr, Expr* cont){
  puts("in Conv Begin");
  Expr* e1 = GetSecond(expr);
  Expr* e2 = GetThird(expr);
  //  PrintExpr(e2);
  //  printf("%d %d\n", e1->type, Number_Exp);
   e2 = Conv(env, e2, cont);
   //  puts("e2 is Conv-ed\n");
   //  PrintExpr(e2);
  Expr* ret, *lambda, *args, *arg;
  ret = malloc(sizeof(Expr));
  lambda = malloc(sizeof(Expr));
  args = malloc(sizeof(Expr));
  arg = malloc(sizeof(Expr));

  ret->type = Pair_Exp;
  ret->u.list = lambda;
  
  lambda->type = Symbol_Exp;
  lambda->u.symbol = "lambda";
  lambda->next = args;

  args->type = Pair_Exp;
  args->u.list = arg;
  args->next = e2;

  char *arg_name = malloc(sizeof(char) * 64);
  sprintf(arg_name, "r_%d", lambda_number++); //lambda argument

  arg->type = Symbol_Exp;
  //  printf("%s\n", arg_name);
  arg->u.symbol = arg_name;
  //    puts("DEBUG");
  arg->next = NullList();
  puts("get out Conv Begin");
  return Conv(env, e1, ret);
}

/* (lambda (P_1 P_2 ...) E_0)
   = convet =>
  (C (lambda (k P_1 P_2 ...) E_0))
                              k
*/
Expr* ConvLambda(Env* env, Expr* expr, Expr* cont){
  Expr* ret = malloc(sizeof(Expr));
  Expr* inside = malloc(sizeof(Expr));
  Expr* lambda_outside = malloc(sizeof(Expr));
  Expr* lambda = malloc(sizeof(Expr));
  Expr* cont_arg = malloc(sizeof(Expr));
  Expr* args = malloc(sizeof(Expr));
  Expr* oldargs;
  Expr* val;

  ret->type = Pair_Exp;
  ret->u.list = cont;
  cont->next = inside;
  ret->next = NullList();
  
  inside->type = Pair_Exp;
  inside->u.list = lambda;
  inside->next = NullList();

  oldargs = expr->next->u.list;

  char* arg_name = malloc(sizeof(char) * 64);
  sprintf(arg_name, "r_%d", lambda_number++); //lambda argument
  //  const char * arg_name = tmp;
  cont_arg->type = Symbol_Exp;
  cont_arg->u.symbol = arg_name;
  cont_arg->next = oldargs;

  val = Conv(env, expr->next->next, dup(cont_arg));

  //  puts("expr->next->next");
  //  PrintExpr(expr->next->next);
  //  puts("val");
  //  PrintExpr(val);
  //    puts("oldargs");
  //  PrintExpr(oldargs);

  //  lambda_outside->type = Pair_Exp;
  //  lambda_outside->u.list = lambda;
  //  lambda_outside->next = val;

  lambda->type = Symbol_Exp;
  lambda->u.symbol = "lambda";
  lambda->next = args;

  args->type = Pair_Exp;
  args->u.list = cont_arg;
  args->next = val; 
  //  puts("RET");
  //  PrintExpr(ret);
  return ret;
}


Expr* ConvPlus(Env* env, Expr* expr, Expr* cont){
  Expr* e1, *e2, *ret,* ret_body,* args1,* arg1, * lambda,*lambda_body,* args2, *arg2,* body, *sum, *sum_body;
  ret = malloc(sizeof(Expr));
  ret_body = malloc(sizeof(Expr));
  args1 = malloc(sizeof(Expr));
  arg1 = malloc(sizeof(Expr));;
  lambda = malloc(sizeof(Expr));
  lambda_body = malloc(sizeof(Expr));
  args2 = malloc(sizeof(Expr));
  arg2 = malloc(sizeof(Expr));
  body = malloc(sizeof(Expr));
  sum = malloc(sizeof(Expr));
  sum_body = malloc(sizeof(Expr));
  e1 = GetSecond(expr);
  e2 = GetThird(expr);
  
  ret->type = Pair_Exp;
  ret->u.list= ret_body;
  ret->next = NullList();

  ret_body->type = Symbol_Exp;
  ret_body->u.symbol = "lambda";
  ret_body->next = args1;

  args1->type = Pair_Exp;
  args1->u.list = arg1;
  //  args1->next

  char *arg1_name = malloc(sizeof(char) * 64);
  sprintf(arg1_name, "r_%d", lambda_number++); //lambda argument
  arg1->type = Symbol_Exp;
  arg1->u.symbol = arg1_name;
  arg1->next = NullList();

  lambda->type = Pair_Exp;
  lambda->u.list = lambda_body;
  lambda->next = NullList();

  lambda_body->type  = Symbol_Exp;
  lambda_body->u.symbol = "lambda";
  lambda_body->next = args2;

  args2->type = Pair_Exp;
  args2->u.list = arg2;
  args2->next = body;

  arg2->type = Symbol_Exp;
  char * arg2_name = malloc(sizeof(char) * 64);
  sprintf(arg2_name, "r_%d", lambda_number++); //lambda argument
  arg2->u.symbol = arg2_name;
  arg2->next = NullList();

  body->type = Pair_Exp;
  body->u.list= cont;
  body->next= NullList();

  cont->next = sum;

  sum->type = Pair_Exp;
  sum->u.list = sum_body;
  sum->next = NullList();

  sum_body->type = Symbol_Exp;
  sum_body->u.symbol = "+";
  sum_body->next = arg1;
  
  arg1->next = arg2;

  args1->next = Conv(env, e2, lambda);

  return Conv(env, e1, ret);
}

/* (define v E)
 =conversion=>
    E_1
    (lambda (r_1) (cont (define v r_1))*/

Expr* ConvDefine(Env* env, Expr* expr, Expr* cont){
  Expr* lambda = malloc(sizeof(Expr));
  Expr* body = malloc(sizeof(Expr));
  Expr* define = malloc(sizeof(Expr));
  Expr* val = malloc(sizeof(Expr));
  Expr* arg = malloc(sizeof(Expr));
  Expr* args = malloc(sizeof(Expr));
  Expr* define_sym = malloc(sizeof(Expr));
  Expr* lambda_body = malloc(sizeof(Expr));

  printf("in conv_define \n"); 
  char *arg_name = malloc(sizeof(char) * 64);
  sprintf(arg_name, "r_%d", lambda_number++); //lambda argument
  printf("%s\n", arg_name);
  Expr* sym_exp = GetSecond(expr);
    puts("DEBUG");
    printf("%d\n", sym_exp->type);
      puts("DEBUG");
  char* sym = sym_exp->u.symbol;  // symbol in "(define symbol value)"
  

  define->type = Pair_Exp;
  define->u.list = define_sym;
  define->next = NullList();

  define_sym->type = Symbol_Exp;
  define_sym->u.symbol = "define";
  define_sym->next = val;

  args->type = Pair_Exp;
  args->u.list = arg;
  args->next = body;
  
  arg->type = Symbol_Exp;
  arg->u.symbol = arg_name;
  arg->next = NullList();
  
  val->type = Symbol_Exp;
  val->u.symbol = sym;
  val->next = arg;
  
  body->type = Pair_Exp;
  body->u.list = cont;
  cont->next = define;
  body->next = NullList();

  lambda->type = Pair_Exp;
  lambda->u.list = lambda_body;
  lambda->next = NullList();
  
  lambda_body->type = Symbol_Exp;
  lambda_body->u.symbol = "lambda";
  lambda_body->next = args;

  printf("%s", arg->u.symbol);
  //  Expr* test = GetThird(expr);
  //  printf("%d", test->u.int_value);
    
  return Conv(env, GetThird(expr), lambda);
 }

Expr* ConvPair(Env* env, Expr* expr, Expr* cont){

  Expr* tmp = expr->u.list;

  printf("symbol is %s in conv_pair\n", tmp->u.symbol);
    if(strcmp(tmp->u.symbol, "define") == 0){
      puts("to conv_define");
      return ConvDefine(env, tmp, cont);
    }else if(strcmp(tmp->u.symbol, "begin") == 0){
      puts("to Conv Begin");
      return ConvBegin(env, tmp, cont);
    }else if(strcmp(tmp->u.symbol, "+") == 0){
      puts(" to conv_plus");
      return ConvPlus(env, tmp, cont);
    }else if(strcmp(tmp->u.symbol, "lambda") == 0){
      printf("lambda");
      return ConvLambda(env, tmp, cont);
    }else if(strcmp(tmp->u.symbol, "if") == 0){
      printf("to conv if");
      return ConvIf(env, tmp, cont);
    }else{
      printf("go conv apply\n");
      return ConvApplyDispatch(env, tmp, cont);
      //      printf("ERROR : cannot this program covert to CPS");
      //      exit(1);
    }
}

Expr* Conv(Env* env, Expr* expr, Expr* cont ){
  Expr* tmp = expr;
  //  printf("in conv\n");
  while(expr != NULL){
    int type = expr->type;
    // printf("expr type is ");
    //    printf("%d%d\n", type, Pair_Exp );
    
    switch(type){
    case Number_Exp :
            printf("go conv_num\n");
      //      printf("DEBUG");
      // PrintExpr(expr);
      return ConvLiteral(env, expr, cont);
    case Symbol_Exp :
      return ConvLiteral(env, expr, cont);
    case Pair_Exp :
             printf("go conv_pair\n");
	     Expr* tmp = ConvPair(env, expr, cont);
	     //	     PrintExpr(tmp);
	     return tmp;
    default :
      return expr;
    }
    
    return tmp;
    tmp = tmp->next;
  }
}
