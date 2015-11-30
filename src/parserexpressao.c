#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include <ctype.h>
#include <stdint.h>
#include "parserexpressao.h"
#include "lex.h"

typedef float real;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned char uchar;

#define TAMVAR          15
#define MAXVARS         15
#define TAMTOK          30

#define VAR             1
#define DEL             2
#define NUM             3

typedef struct
{
    char nome[TAMVAR + 1];
    double valor;
} VARIAVEL;

typedef struct
{
    char* nome;
    int   args;
    double(*func)();
} FUNCAO;

#define iswhite(c)  (c == ' ' || c == '\t')
#define isnumer(c)  ((c >= '0' && c <= '9') || c == '.')
#define isdelim(c)  (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' \
                  || c == '^' || c == '(' || c == ')' || c == ',' || c == '=')

#define ERR(n) {ERRO=n; ERPOS=expressao-ERANC-1; strcpy (ERTOK,token); longjmp(jb,1);}

// prototipos
static int level1(double *r);
static void level2(double *r);
static void level3(double *r);
static void level4(double *r);
static void level5(double *r);
static void level6(double *r);

// Prototipos para as funções "custom" do programa
double deg(double x);
double rad(double x);
double sqrt_p(double x);
double log_p(double x);
double ln_p(double x);

#define PI    3.14159265358979323846
#define M_E   2.71828182845904523536

#define DPR (180./PI)
#define RPD (PI/180.)

int   ERRO;               // Numero do erro
char  ERTOK[TAMTOK + 1];  // Token gerador do erro...
int   ERPOS;              // A posição do cursor, com base no inicio da expr
unsigned char* ERANC;     // Usado para calcular a pos. do erro

VARIAVEL constantes[] =
{
    { "pi",      PI },
    { "e",       M_E },
    { "dpr",     DPR },
    { "rpd",     RPD },
    { 0 }
};

FUNCAO funcoes[] =
{
    { "sin",     1,    sin },
    { "sen",     1,    sin },
    { "cos",     1,    cos },
    { "tan",     1,    tan },
    { "tg",      1,    tan },
    { "asin",    1,    asin },
    { "asen",    1,    asin },
    { "acos",    1,    acos },
    { "atan",    1,    atan },
    { "atg",     1,    atan },
    { "exp",     1,    exp },
    { "ln",      1,    ln_p },
    { "log",     1,    log_p },
    { "sqrt",    1,    sqrt_p },
    { "sqr",     1,    sqrt_p },
    { "floor",   1,    floor },
    { "ceil",    1,    ceil },
    { "abs",     1,    fabs },
    { "hypot",   2,    hypot },
    { "rss",     2,    hypot },
    { "deg",     1,    deg },
    { "rad",     1,    rad },
    { 0 }
};

VARIAVEL        variaveis[MAXVARS];
unsigned char*  expressao;
unsigned char   token[TAMTOK + 1];
int             tipo;
jmp_buf         jb;

double deg(double x)
{
    return (x*DPR);
}

double rad(double x)
{
    return (x*RPD);
}

double sqrt_p(double x)
{
	if(x < 0)
		ERR(E_RAIZNEG);
	return sqrt(x);
}

double log_p(double x)
{
	if(x <= 0)
		ERR(E_LOGINV);
	return log10(x);
}

double ln_p(double x)
{
	if(x <= 0)
		ERR(E_LNINV)
	return log(x);
}

int pegasimbolo(char *s, double *v)
{
    char *e;

    e = getenv(s);
    if (!e) return 0;
    *v = atof(e);
    return 1;
}

void limpatodasasvariaveis()
{
    int i;

    for (i = 0; i < MAXVARS; i++)
        *variaveis[i].nome = 0;
    variaveis[i].valor = 0;
}

int limpavariavel(char *nome)
{
    int i;

    for (i = 0; i < MAXVARS; i++)
        if (*variaveis[i].nome && !strcmp(nome, variaveis[i].nome))
        {
            *variaveis[i].nome = 0;
            variaveis[i].valor = 0;
            return 1;
        }
    return 0;
}

int pegavariavel(char *nome, double *valor)
{
    int i;

    if (*nome == '_')
        return (pegasimbolo(nome + 1, valor));

    for (i = 0; i < MAXVARS; i++)
        if (*variaveis[i].nome && !strcmp(nome, variaveis[i].nome))
        {
            *valor = variaveis[i].valor;
            return 1;
        }

    for (i = 0; *constantes[i].nome; i++)
        if (*constantes[i].nome && !strcmp(nome, constantes[i].nome))
        {
            *valor = constantes[i].valor;
            return 1;
        }
    return 0;
}

int setavariavel(char *nome, double *valor)
{
    int  i;

    limpavariavel(nome);
    for (i = 0; i < MAXVARS; i++)
        if (!*variaveis[i].nome)
        {
            strcpy(variaveis[i].nome, nome);
            variaveis[i].nome[TAMVAR] = 0;
            variaveis[i].valor = *valor;
            return 1;
        }
    return 0;
}

static void parse()
{
    char *t;

    tipo = 0;
    t = token;
    while (iswhite(*expressao))
        expressao++;
    if (isdelim(*expressao))
    {
        tipo = DEL;
        *t++ = *expressao++;
    }
    else if (isnumer(*expressao))
    {
        tipo = NUM;
        while (isnumer(*expressao))
            *t++ = *expressao++;
    }
    else if (isalpha(*expressao))
    {
        tipo = VAR;
        while (isalpha(*expressao))
            *t++ = *expressao++;
        token[TAMVAR] = 0;
    }
    else if (*expressao)
    {
        *t++ = *expressao++;
        *t = 0;
        ERR(E_SYNTAX);
    }
    *t = 0;
    while (iswhite(*expressao))
        expressao++;
}

/* level1..----------------------------------------------------------------
** Esse level trata da operação de setar uma variavel, pela expressao
** ou seja:
**      x = 10
** o sistema vai identificar que você quer fazer X = 10, então ele vai
** criar essa variavel no sitema, ou se não, apenas alterar o seu valor!
*
** Ele retorna um valor de 1 se for uma operação de atribuição de nível
** superior, caso contrário ele retorna 0
*/
static int level1(double *r)
{
    char t[TAMVAR + 1];

    if (tipo == VAR)
        if (*expressao == '=')
        {
            strcpy(t, token);
            parse();
            parse();
            if (!*token)
            {
                limpavariavel(t);
                return 1;
            }
            level2(r);
            if (!setavariavel(t, r))
                ERR(E_MAXVARS);
            return 1;
        }
    level2(r);
    return 0;
}

/* level2..----------------------------------------------------------------
** Esse level trata da operação de soma ou subtração
*/
static void level2(double *r)
{
    double t = 0;
    char o;

    level3(r);
    while ((o = *token) == '+' || o == '-')
    {
        parse();
        level3(&t);
        if (o == '+')
            *r = *r + t;
        else if (o == '-')
            *r = *r - t;
    }
}

/* level3..----------------------------------------------------------------
** Esse level trata de todoas as multiplicações ou divisões ou modulo!
*/
static void level3(double *r)
{
    double t;
    char o;

    level4(r);
    while ((o = *token) == '*' || o == '/' || o == '%')
    {
        parse();
        level4(&t);
        if (o == '*')
            *r = *r * t;
        else if (o == '/')
        {
            if (t == 0)
                ERR(E_DIVZERO);
            *r = *r / t;
        }
        else if (o == '%')
        {
            if (t == 0)
                ERR(E_DIVZERO);
            *r = fmod(*r, t);
        }
    }
}

/* level4..----------------------------------------------------------------
** Essa level trata somente de numeros elevados!
*/
static void level4(double *r)
{
    double t;

    level5(r);
    if (*token == '^')
    {
        parse();
        level5(&t);
        *r = pow(*r, t);
    }
}

/* level5..----------------------------------------------------------------
** Esse level pega todos os sinais unarios ( + ou - )
*/
static void level5(double *r)
{
    char o = 0;

    if (*token == '+' || *token == '-')
    {
        o = *token;
        parse();
    }
    level6(r);
    if (o == '-')
        *r = -*r;
}

/* level6..----------------------------------------------------------------
** Esse level pega todos os literais numericos, variaveis ou funções...
*/
static void level6(double *r)
{
    int  i, n;
    double a[3];
    int tamanhoTabelaFuncoes = sizeof(funcoes) / sizeof(FUNCAO);

    if (*token == '(')
    {
        parse();
        if (*token == ')')
            ERR(E_SEMARQ);
        level1(r);
        if (*token != ')')
            ERR(E_FALTPAR);
        parse();
    }
    else
    {
        if (tipo == NUM)
        {
            *r = (double)atof(token);
            parse();
        }
        else if (tipo == VAR)
        {
            if (*expressao == '(')
            {
                for (i = 0; i < tamanhoTabelaFuncoes; i++)
                {
                    if (funcoes[i].nome == NULL)
                        break;

                    if (!strcmp(token, funcoes[i].nome))
                    {
                        parse();
                        n = 0;
                        do
                        {
                            parse();
                            if (*token == ')' || *token == ',')
                                ERR(E_SEMARQ);
                            a[n] = 0;
                            level1(&a[n]);
                            n++;
                        }
                        while (n < 4 && *token == ',');
                        parse();
                        if (n != funcoes[i].args)
                        {
                            strcpy(token, funcoes[i].nome);
                            ERR(E_NUMARGS);
                        }
                        *r = funcoes[i].func(a[0], a[1], a[2]);
                        return;
                    }
                }
                if (funcoes[i].nome == NULL)
                    ERR(E_FUNCNLO);
            }
            else if (!pegavariavel(token, r))
                ERR(E_DESCONHECIDO);
            parse();
        }
        else
            ERR(E_SYNTAX);
    }
}

#if !defined(_MSC_VER)
char *_strlwr(char *str)
{
    char *ret = str;
    while (*str != '\0')
    {
        if (isupper(*str))
            *str = tolower(*str);
        ++str;
    }
    return ret;
}
#endif

int isnan_d(double d)
{
    //const uint64_t u = *(uint64_t*)&d;
    //return (u&0x7FF0000000000000ULL) == 0x7FF0000000000000ULL && (u&0xFFFFFFFFFFFFFULL);
    int hx,lx;
    hx = (__HI(d)&0x7fffffff);
    lx = __LO(d);
    hx |= (unsigned)(lx|(-lx))>>31;
    hx = 0x7ff00000 - hx;
    return ((unsigned)(hx))>>31;
}

/*int isinf_d(double d)
{
	int hx;
	hx = __HI(d);
	return  (unsigned)((hx & 0x7fffffff) - 0x7ff00000) >> 31;
}*/

int isinf_d(double d)
{
	return isinf(d);
}

int calcula(char *expr, double *resultado, int *flag)
{
    if (setjmp(jb))
        return ERRO;

    expressao = expr;
    ERANC = expr;
    _strlwr(expressao);
    *resultado = 0;
    parse();
    if (!*token)
        ERR(E_VAZIA);
    *flag = level1(resultado);
	if(isnan_d(*resultado))
		return E_NAN;
	if (isinf_d(*resultado))
		return E_INF;

    return E_OK;
}
