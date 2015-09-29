#ifndef __PARSEREXPRESSAO_H__
#define __PARSEREXPRESSAO_H__

typedef enum
{
	divisao_por_zero,
	expressao_vazia,
	parenteses_nao_coincidem,
	parenteses_direito,
	tamanho_maximo,
	fatorial_nao_pode_zero,
	raiz_negativa,
	log_zero,
	ln_ep,
	tangente_noventa,
	erro_sintaxe,
	erro_analisador,
	ok = 99,
}CodigoErro;

static const char *MensagemDoErro[] = 
{
	"Divisao por zero",
	"Expressao vazia",
	"Os parenteses nao coincidem",
	"Faltou um ')'",
	"Comprimento expressões eh maior que o maximo",
	"Para calcular o fatorial o numero deve ser maior que 0",
	"Impossivel calcular raiz quadrada de numero menores que 0",
	"Impossivel calcular o Log com valores menores ou iguais a 0",
	"O valor de LN tem que ser maior que EPSILON",
	"Impossivel calcular a tangente de valores >= 90 graus",
	"Erro de sintaxe",
	"Erro do analisador de expressao"
};

typedef double(*UserFunction)(double value);
int adicionaVariavelDoUsuario(const char *name, double value);
void setaValorDaVariavel(const char *name, double value);
double pegaValorDaVariavel(const char *name);
void incrementaValor(const char *name, double value);
void decrementaValor(const char *name, double value);
int adicionaConstanteDoUsuario(const char *name, double value);
void registraFuncaoDoUsuario(char *funcName, UserFunction proc);
double calcula(const char *formula);
int TemErro();
CodigoErro PegaCodigoErro();

#endif
