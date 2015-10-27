#if defined(_MSC_VER)
#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lex.h"

#define LINESIZE        2048    
#define NTOKEN          100000  
#define MAXLINES        10000   

char *token[NTOKEN];
int   nToken;
int   numToken[MAXLINES];

#define OPERATOR2 "== != <= >= >> << && || += -= *= /= ++ -- [] " 

void analiselexica(unsigned char *linebuffer, int printLexemas)
{
	char buf[LINESIZE + 1], *p, *pBgn;
	int  lenToken;

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
			if (*p == '0' && (p[1] == 'x' || p[1] == 'X'))
				strtol(p, &p, 16);
			else
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

		token[nToken++] = _strdup(buf);
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

void processaexpressao(unsigned char *expr)
{
	int i = 0;
	const int tamAnte = 128;
	unsigned char *expt = calloc(sizeof(unsigned char), LINESIZE);
	unsigned char *ante = calloc(sizeof(unsigned char), tamAnte);
	memset(expt, '\0', LINESIZE);
	memset(ante, '\0', tamAnte);

	for (; i<nToken; i++)
	{
		if ((isalpha(token[i][0]) && isdigit(ante[0])) || isalpha(ante[0]) && token[i][0] == '(')
		{
			expt = concat(expt, "*");
			expt = concat(expt, token[i]);
		}
		else if (isdigit(token[i][0]) && isalpha(ante[0]))
		{
			expt = concat(expt, "^");
			expt = concat(expt, token[i]);
		}
		else
			expt = concat(expt, token[i]);

		ante = token[i];
	}
	memcpy(expr, expt, strlen(expt));
	free(expt);
	free(ante);
}