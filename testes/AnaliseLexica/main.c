#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define LINESIZE        1024    
#define NTOKEN          100000  
#define MAXLINES        10000   

char *token[NTOKEN];
int   nToken;
int   numToken[MAXLINES];

#define OPERATOR2 "== != <= >= >> << && || += -= *= /= ++ -- [] " 

void analiselexica(char *linebuffer, int printLexemas)
{
	char buf[LINESIZE + 1], *p, *pBgn;
	int  k, lenToken;

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
		if(printLexemas == 1)
			printf("[%3d]: %s\n", nToken, buf);
	}
}

char* concat(char *s1, char *s2)
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

void processaexpressao(char *expr)
{
  int i = 0;
  const int tamAnte = 128;
  char *ante = malloc(sizeof(char) * tamAnte);
	memset(ante, '\0', tamAnte);

	for (; i<nToken; i++)
	{
		if ((isalpha(token[i][0]) && isdigit(ante[0])) || isalpha(ante[0]) && token[i][0] == '(')
		{
			expr = concat(expr, "*");
			expr = concat(expr, token[i]);
		}
		else if (isdigit(token[i][0]) && isalpha(ante[0]))
		{
			expr = concat(expr, "^");
			expr = concat(expr, token[i]);
		}
		else
			expr = concat(expr, token[i]);

		ante = token[i];
	}
  free(ante);
}

int main(int argc, char* argv[])
{
	int i = 0;
	char *teste = "(-x(2+29x)+2x)/(x(2+2))";
 
  const int tamExpr = 1048; 
	char *expr = malloc(sizeof(char) * tamExpr);
  memset(expr, '\0', tamExpr);

	analiselexica(teste, 0);
  processaexpressao(expr);
	
	printf("%s\n", expr);
	free(expr);
	return 0;
}

