#include <stdio.h>
#include <string.h>
#include "parserexpressao.h"

int main(int argc, char *argv[])
{
	int flag = 0, errorCode = 10;
	double result;
	double valor = 2;
	char *expr = (char*)calloc(sizeof(char), 1024);
	unsigned char *tmp = (unsigned char*)calloc(sizeof(unsigned char), 2048);
	
	
	do
	{
		memset(expr, '\0', 2048);
		printf("Digite uma expressao: ");
		scanf("%s", expr);
	
		// TODO
		memset(tmp, '\0', 2048);
		memcpy(tmp, expr, strlen(expr));
		analiselexica(tmp, 0);
		memset(tmp, '\0', 2048);
		processaexpressao(tmp, 0);
			
		setavariavel("x", &valor);
		errorCode = calcula(tmp, &result, &flag);
		printf("\nExpressão: %s\n", expr);
		printf("Expressão processada: %s\n", tmp);
		printf("Codigo de erro: %d\n", errorCode);
		if (errorCode != E_OK)
			printf("Mensagem de erro: %s\n", mensagensDeErro[errorCode - 1]);
		printf("Flag: %d\n", flag);
		printf("Resultado: %f\n\n\n", result);
	}while(strcmp(expr, "-q"));

	return 0;
}