#include <stdio.h>
#include <windows.h>
#include "ParserExpressao.h"

double Multiplica_por_4(double value)
{
	return value * 4;
}

int main(int argc, char *argv[])
{
	double result;

	char *expr = "+1+3-((2*10)-5) +(five*var)-sqr(((1+1)*sqr(2)))+Multiplica_por_4(3)+PI*2.34+abs(-10.2)+int(-20)";
	adicionaConstanteDoUsuario("five", 5);
	adicionaVariavelDoUsuario("var", 15);
	registraFuncaoDoUsuario("Multiplica_por_4", Multiplica_por_4);

	DWORD ticks1 = GetTickCount();
	for (int aaa = 0;aaa < 20000; aaa++)
		result = calcula(expr);
	DWORD ticks2 = GetTickCount();
	printf("Resultado: %f\nTempo de calculo para 2000 interacoes: %d\n", result, ticks2 - ticks1);


	return 0;
}

