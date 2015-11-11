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
#define E_NAN          13     // Not a Number	

static const char* mensagensDeErro[] =
{
   "Erro de sintaxe",
   "Falta parêntese",
   "Divisão por zero",
   "Variável desconhecida",
   "Número máximo de variáveis excedido",
   "Função não reconhecida",
   "Número incorreto de argumentos para a função",
   "Está faltando argumentos",
   "Expressão vazia",
   "x não pode ser negativo para raiz quadrada",
   "x não pode ser menor ou igual a 0 para log10",
   "x não pode ser menor ou igual a 0 para ln",
   "Erro no resultado"
};

int setavariavel(char *nome, double *valor);
int calcula(char *expr, double *resultado, int *flag);

#endif
