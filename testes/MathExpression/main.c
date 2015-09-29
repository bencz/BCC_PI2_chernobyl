#include <stdio.h>
//#include <windows.h>
#include "ParserExpressao.h"

double Multiplica_por_4(double value)
{
	return value * 4;
}

int main(int argc, char *argv[])
{
	double result;

	result = calcula("log(-1)");
	
	if(TemErro() && PegaCodigoErro() != ok)
	{
		//printf("%f\n", result);
		printf("Erro: %s\n", MensagemDoErro[PegaCodigoErro()]);
	}
	//printf("Resultado: %f\n", result);


	return 0;
}

