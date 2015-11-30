#if defined(_MSC_VER)
#define _STRDUP_ _strdup
#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#else
#define _STRDUP_ strdup
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "lex.h"

#define LINESIZE        2048
#define NTOKEN          1000
#define MAXLINES        1000

char *token[NTOKEN];
int   nToken;
int   numToken[MAXLINES];

#define OPERATOR2 "== != <= >= >> << && || += -= *= /= ++ -- [] "

void analiselexica(unsigned char *linebuffer, int printLexemas)
{
	nToken = 0;
	char buf[LINESIZE + 1], *p, *pBgn;
	int  lenToken;
	memset(token, '\0', 2048);

	p = linebuffer;
	while (*p != '\0')
	{
		if (*p <= ' ')
		{
			p++;
			continue;
		}

		if (*p == '#')
			return;

		if (*p == '/' && p[1] == '/')
			return;

		pBgn = p;
		if (isalpha(*p) || *p == '_')             // [_A-Za-z][_A-Za-z0-9]*
		{
			for (p++; isalpha(*p) || *p == '_'; )
				p++;
		}
		else if (isdigit(*p))
		{
			//if (*p == '0' && (p[1] == 'x' || p[1] == 'X'))
			//	strtol(p, &p, 16);
			//else
				strtod(p, &p);
		}
		else if (*p == '\"' || *p == '\'')
		{
			for (p++; *p != '\0' && *p != *pBgn; p++)
			{
				if (*p == '\\')
					p++;
			}
			if (*p++ != *pBgn)
			{
				fprintf(stderr, "Faltou as aspas (%c)", *pBgn);
				exit(0);
			}
		}
		else
		{
			char op2[4];
			sprintf(op2, "%c%c ", p[0], p[1]);
			if (strstr(OPERATOR2, op2) != NULL) p += 2;
			else p++;
		}
		lenToken = p - pBgn;
		if (*pBgn == '"' || *pBgn == '\'') lenToken--;
		memcpy(buf, pBgn, lenToken);
		buf[lenToken] = '\0';

		token[nToken++] = _STRDUP_(buf);
		if (printLexemas == 1)
			printf("[%3d]: %s\n", nToken, buf);
	}
}

unsigned char* concat(unsigned char *s1, unsigned char *s2)
{
	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);
	char *result = malloc(len1 + len2 + 1);//+1 para o terminador 0
	if (result == NULL)
		printf("Falha ao alocar memoria! :(");

	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2 + 1);//+1 para copiar o terminador 0
	return result;
}

char *funcs[] =
{
	"sin",
	"sen",
	"cos",
	"tan",
	"tg",
	"asin",
	"asen",
	"acos",
	"atan",
	"atg",
	"exp",
	"ln",
	"log",
	"sqrt",
	"sqr",
	"floor",
	"ceil",
	"abs",
	"hypot",
	"rss",
	"deg",
	"rad"
};

int funcaoExiste(char *func)
{
	int i = 0, b = 18;
	for (; i < b; i++)
	{
		if (!strcmp(func, funcs[i]))
			return 1;
	}
	return 0;
}

int processaexpressao(unsigned char *expr, int i)
{
	const int tamAnte = 1024;
	unsigned char *expt = calloc(sizeof(unsigned char), LINESIZE);
	unsigned char *ante = calloc(sizeof(unsigned char), tamAnte);
	memset(expt, '\0', LINESIZE);
	memset(ante, '\0', tamAnte);

	for (; i < nToken; i++)
	{
		if ((isalpha(token[i][0]) && isdigit(ante[0])) || isalpha(ante[0]) && token[i][0] == '(')
		{
			// coloquei a função aqui dentro para que ele nao entre no FOR
			// da função funcaoExiste, toda vez que passar pelo if aqui de cima!
			if (!funcaoExiste(token[i - 1]))
			{
				expt = concat(expt, "*");
				expt = concat(expt, token[i]);
			}
			else
				expt = concat(expt, token[i]);
		}

		else if (isdigit(ante[0]) && token[i][0] == '(')
		{
			// coloquei a função aqui dentro para que ele nao entre no FOR
			// da função funcaoExiste, toda vez que passar pelo if aqui de cima!
			if (!funcaoExiste(token[i - 1]))
			{
				expt = concat(expt, "*");
				expt = concat(expt, token[i]);
			}
			else
				expt = concat(expt, token[i]);
		}

		else if (isdigit(token[i][0]) && isalpha(ante[0]))
		{
			expt = concat(expt, "^");
			expt = concat(expt, token[i]);
		}
		else if ((token[i + 1] != NULL && token[i - 1] != NULL) &&
				 (isdigit(token[i + 1][0]) && isalpha(token[i][0]) && token[i - 1][0] == '-'))
		{
			// -x2
			expt = concat(expt, "(");
			expt = concat(expt, token[i]);
			expt = concat(expt, "^");
			expt = concat(expt, token[i+1]);
//			unsigned char *tmp = calloc(sizeof(unsigned char), LINESIZE);
//			i = (processaexpressao(tmp, i + 1)) - 1;
//			expt = concat(expt, tmp);
			expt = concat(expt, ")");
			i += 1;
//			free(tmp);
			continue;
		}
		else if ((token[i + 1] != NULL && token[i - 1] != NULL) &&
				 (token[i + 1][0] == '^' && token[i - 1][0] == '-'))
		{
			expt = concat(expt, "(");
			expt = concat(expt, token[i]);
			expt = concat(expt, token[i + 1]);
			i += 1;
			/*if (token[i + 2] != NULL && (token[i + 2][0] == '('))
			{
				unsigned char *tmp = calloc(sizeof(unsigned char), LINESIZE);
				i = (processaexpressao(tmp, i + 2)) - 1;
				expt = concat(expt, tmp);
				expt = concat(expt, ")");
				free(tmp);
				continue;
			}
			else*/ if(expt, token[i + 1] != NULL)
			{
				expt = concat(expt, token[i + 1]);
				i += 1;
				if (token[i + 1] != NULL && (isdigit(token[i + 1][0])))
					expt = concat(expt, token[i + 1]);
				expt = concat(expt, ")");
				i += 1;
				ante = token[i];
				continue;
			}
		}
		else
			expt = concat(expt, token[i]);

		ante = token[i];
	}
	memcpy(expr, expt, strlen(expt));
	free(expt);
	free(ante);

#ifdef _WIN32
	//free(token);
	memset(token, '\0', NTOKEN);
#elif __linux__
	memset(token, '\0', NTOKEN);
#endif
	return i;
}
