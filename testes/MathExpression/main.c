#include <stdio.h>
#include "parserexpressao.h"

int main(int argc, char *argv[])
{
	int flag = 0, errorCode = 10;
	double result;
	double valor = 2;
	char *expr = "(-x^(x+2x))/sin(((2x)))";

	setavariavel("x", &valor);
	errorCode = calcula(expr, &result, &flag);

	printf("Expressão: %s\n", expr);
	printf("Codigo de erro: %d\n", errorCode);
	if (errorCode != E_OK)
		printf("Mensagem de erro: %s\n", mensagensDeErro[errorCode - 1]);
	printf("Flag: %d\n", flag);
	printf("Resultado: %f\n", result);

	return 0;
}