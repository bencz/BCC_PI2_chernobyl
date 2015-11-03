#ifndef __PARSEREXPRESSAO_H__
#define __PARSEREXPRESSAO_H__

// Codigos que são retornados pelo sistema de calculo 
#define E_OK            0     // Calculo feito com sucesso 
#define E_SYNTAX        1     // Erro se sintaxe 
#define E_FALTPAR       2     // Falta parentese 
#define E_DIVZERO       3     // Divisao por zero 
#define E_DESCONHECIDO  4     // Variavel desconhecida 
#define E_MAXVARS       5     // Numero maximo de variaveis excedido 
#define E_FUNCNLO       6     // Funcao nao reconhecida 
#define E_NUMARGS       7     // Numero incorreto de argumentos para a funcao 
#define E_SEMARQ        8     // Esta faltando argumentos 
#define E_VAZIA         9     // Expressao vazia 
#define E_RAIZNEG      10     // Erro de raiz negativa
#define E_LOGINV	   11     // Erro para log menor ou igual à zero
#define E_LNINV        12     // Erro para ln menor ou igual à zero

static const char* mensagensDeErro[] =
{
   "Erro se sintaxe",
   "Falta parentese",
   "Divisao por zero",
   "Variavel desconhecida",
   "Numero maximo de variaveis excedido",
   "Funcao nao reconhecida",
   "Numero incorreto de argumentos para a funcao",
   "Esta faltando argumentos",
   "Expressao vazia",
   "x não pode ser negativo para raiz quadrada",
   "x não pode ser menor ou igual à 0 para log10",
   "x não pode ser menor ou igual à 0 para ln"
};

int setavariavel(char *nome, double *valor);
int calcula(char *expr, double *resultado, int *flag);

#endif
