#include"scheme.h"
#include"conv.h"
Expr* cont_endpoint;
int lambda_number;


Expr* ConvBegin(Env* env, Expr* expr, Expr* cont){
  puts("in Conv Begin");
  Expr* e1 = GetSecond(expr);
  Expr* e2 = GetThird(expr);
  PrintExpr(e2);
  printf("%d %d\n", e1->type, Number_Exp);
   e2 = Conv(env, e2, cont);
  puts("e2 is Conv-ed\n");
  PrintExpr(e2);
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

  char arg_name[100];
  sprintf(arg_name, "r_%d", lambda_number++); //lambda argument

  arg->type = Symbol_Exp;
  printf("%s\n", arg_name);
  arg->u.symbol = arg_name;
    puts("DEBUG");
  arg->next = NullList();
  puts("get out Conv Begin");
  return Conv(env, e1, ret);
}




Expr* ConvNum(Env* env, Expr* expr, Expr* cont){
  Expr* ret = malloc(sizeof(Expr));
  ret->type = Pair_Exp;
  ret->u.list = cont;
  cont->next = expr;
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

  char arg1_name[100];
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
  char arg2_name[100];
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
  char arg_name[100];
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
  lambda->u.symbol = lambda_body;
  lambda->next = NullList();
  
  lambda_body->type = Symbol_Exp;
  lambda_body->u.symbol = "lambda";
  lambda_body->next = args;

  //  Expr* test = GetThird(expr);
  //  printf("%d", test->u.int_value);
    
  return Conv(env, GetThird(expr), lambda);
 }

Expr* ConvPair(Env* env, Expr* expr, Expr* cont){

  Expr* tmp = expr->u.list;

//  printf("symbol is %s in conv_pair\n", tmp->u.symbol);
    if(strcmp(tmp->u.symbol, "define") == 0){
      puts("to conv_define");
      return ConvDefine(env, tmp, cont);
    }else if(strcmp(tmp->u.symbol, "begin") == 0){
      puts("to Conv Begin");
      return ConvBegin(env, tmp, cont);
    }else if(strcmp(tmp->u.symbol, "+") == 0){
      return ConvPlus(env, tmp, cont);
    }else{
      printf("ERROR : cannot this program covert to CPS");
      exit(1);
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
      return ConvNum(env, expr, cont);
    case Pair_Exp :
             printf("go conv_pair\n");
      return ConvPair(env, expr, cont);
    default :
      return expr;
    }
    
    return tmp;
    tmp = tmp->next;
  }
}
