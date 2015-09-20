#ifndef __PARSEREXPRESSAO_H__
#define __PARSEREXPRESSAO_H__

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

#endif
