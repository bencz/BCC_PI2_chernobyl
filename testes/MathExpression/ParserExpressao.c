#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "parserExpressao.h"

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

static bool isAddOp(char c)
{
  return (c == '+' || c == '-'); 
}

static bool isMulOp(char c) 
{ 
  return (c == '*' || c == '/' || c == '^');
}

static bool isNumeric(char c)
{
  return (isdigit(c) || c == '.' || c == ',');
}

static bool isLetter(char c)
{
  return (isalpha(c) || c == '_');
}

static bool hasError;

static char substring(const char *entrada, int offset, int tamanho, char *destino)
{
  int tamanho_input = (int)strlen(entrada);
  if(offset + tamanho > tamanho_input)
    return NULL;
  
  strncpy(destino, entrada + offset, tamanho);
  return destino;
}

static void strtrim(char *str, char *saida)
{
  int i = 0;
  int j = 0;
  int tamanho_input = (int)strlen(str);
  
  for(i = 0;i<tamanho_input;i++)
  {
    if(str[i] != ' ' && str[i] != '\t')
      saida[j++] = str[i];
  }
  saida[j] = '\0';
}

static double fatorial(int valor)
{
  double resultado;
  resultado = (valor <= 1) ? valor : (valor * fatorial(valor -1));
  return resultado;
}

static double fator()
{
  return 0;
}

static double termo()
{
  double valor;
  double tmpValor;
  
  valor = fator();
}

static double expressao()
{
  double valor;
  valor = termo();
}
                                              
double calcula(const char *formula)
{
}  

int temErro()
{
  return temErro;
}