#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "ParserExpressao.h"

/*
{ BNF:
  Expression::=<Term> [<AddOp> <Term>]*
  <Term>    ::=<Factor> [<MulOp> <Factor> ]*
  <Factor>  ::=(<Expression>) | <Numeric> | <Constant> | UserFunc | PreDefinedFunc
}
*/

#define PI    3.14159265359
#define E     2.71828182845

#define TAM_MAX_FORMULA       256*4
#define TAM_MAX_CONSTANTES    256
#define TAM_MAX_VARIAVEIS     256
#define TAM_MAX_NOME          256
#define TAM_MAX_FUNCOES       256

static bool hasError;

static double Fator()
{
  return 0;
}

static double Termo()
{
  double valor;
  double tmpValor;
  
  valor = Fator();
}

static double Expressao()
{
  double valor;
  valor = Termo();
}
                                              
double Calcula(const char *formula)
{
}  

int TemErro()
{
  return temErro;
}