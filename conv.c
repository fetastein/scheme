#include"scheme.h"
#include"conv.h"

Expr* ConvNum(Env* env, Expr* expr, Expr* cont){
  Expr* ret = malloc(sizeof(Expr));
  ret->type = Pair_Exp;
  ret->u.list = cont;
  cont->next = expr;
  return ret;
}

Expr* ConvPair(Env* env, Expr* expr, Expr* cont){
  printf("in conv pair\n");
  Expr* tmp = expr->u.list;
  printf("in conv pair2 \n");
  printf("symbol is %s in conv_pair\n", tmp->u.symbol);
  if(strcmp(tmp->u.symbol, "define") == 0){
    puts("to conv_define");
    return ConvDefine(env, tmp, cont);
  }else{
    printf("ERROR : cannot this program covert to CPS");
    exit(1);
  }
}
  
int lambda_number = 0;

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
  Expr* lambda_body = main(sizeof(Expr));

  printf("in conv_define \n"); 
  char arg_name[100];
  sprintf(arg_name, "r_%d", lambda_number++); //lambda argument
  puts("DEBUG");
  Expr* sym_exp = GetSecond(expr);
    puts("DEBUG");
    printf("%d\n", sym_exp->type);
      puts("DEBUG");
  char* sym = sym_exp->u.symbol;  // symbol in "(define symbol value)"
  

  define->type = Pair_Exp;
  define->u.list = define_sym;
  define->next = val;

  define_sym->type = Symbol_Exp;
  define_sym->u.symbol = "define";
  define_sym->next = val;

  args->type = Pair_Exp;
  args->u.list = arg;
  arg->next = body;
  
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

Expr* Conv(Env* env, Expr* expr, Expr* cont ){
  Expr* tmp = expr;
  printf("in conv\n");
  while(expr != NULL){
    int type = expr->type;
    printf("expr type is ");
    printf("%d%d\n", type, Pair_Exp );
    
    switch(type){
    case Number_Exp :
      printf("go conv_num\n");
      printf("DEBUG");
      PrintExpr(expr);
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
