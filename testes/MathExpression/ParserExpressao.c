#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include "ParserExpressao.h"

#if (defined(_WIN32) || defined(_WIN64)) && defined(_MSC_VER)
#define strcasecmp _stricmp
#endif

/*
{ BNF:
  Expression::=<Term> [<AddOp> <Term>]*
  <Term>    ::=<Factor> [<MulOp> <Factor> ]*
  <Factor>  ::=(<Expression>) | <Numeric> | <Constant> | UserFunc | PreDefinedFunc
}
*/
#define PI    3.14159265359
#define E     2.71828182845

#define EPSILON    1.19e-7f

#define IsAddOp(c)    ((c) == '+' || (c) == '-')
#define IsMulOp(c)    ((c) == '*' || (c) == '/' || (c) == '^')
#define IsNumeric(c)  (isdigit((c)) || (c) == '.')
#define IsLetter(c)   (isalpha((c)) || (c) == '_')

#define MAX_FORMULA_LEN  1024 
#define MAX_CONSTS       256  
#define MAX_VARIABLE     256  
#define MAX_NAME         256  
#define MAX_FUNCS        256               

typedef struct _VarTable ConstTable;
typedef struct _VarTable VarTable;

struct _VarTable
{
	char name[MAX_NAME];
	double value;
};

typedef enum TParserFunc
{
	pfArcTan, pfCos, pfSin, pfTan, pfAbs, pfExp, pfLn, pfLog,
	pfSqrt, pfSqr, pfInt, pfRound, pfFloor, pfCeiling,
	pfArcSin, pfArcCos, pfSign, pfFact
}TParserFunc;

typedef struct FuncTable
{
	TParserFunc func;
	const char *name;
}FuncTable;

typedef struct UserFuncList
{
	char name[MAX_NAME];
	UserFunction func;
}UserFuncList;

FuncTable funcTables[] =
{
  {pfArcTan,  "ARCTAN"},
  {pfCos,     "COS"},
  {pfSin,     "SIN"},
  {pfTan,     "TAN"},
  {pfAbs,     "ABS"},
  {pfExp,     "EXP"},
  {pfLn,      "LN"},
  {pfLog,     "LOG"},
  {pfSqrt,    "SQRT"},
  {pfSqr,     "SQR"},
  {pfInt,     "INT"},
  {pfRound,   "ROUND"},
  {pfFloor,   "FLOOR"},
  {pfCeiling, "CEILING"},
  {pfArcSin,  "ARCSIN"},
  {pfArcCos,  "ARCCOS"},
  {pfSign,    "SIGN"},
};

#define FuncTableSize sizeof(funcTables) /sizeof(funcTables[0])

static ConstTable constTable[MAX_CONSTS];
static int nConsts;

static UserFuncList userFuncList[MAX_FUNCS];
static int nUserFuncs;

static VarTable varTable[MAX_VARIABLE];
static int nVars;

static bool temErro;
static int iCurPos;
static char FParseText[MAX_FORMULA_LEN];

static double Expression();

char* SubString(const char* input, int offset, int len, char* dest)
{
	int input_len = (int)strlen(input);

	if (offset + len > input_len)
	{
		return NULL;
	}

	strncpy(dest, input + offset, len);
	return dest;
}

void StrTrim(char *str, char *out)
{
	int i, j = 0;
	int end = (int)strlen(str);

	for (i = 0; i < end; i++)
	{
		if (str[i] != ' ' && str[i] != '\t')
			out[j++] = str[i];
	}

	out[j] = '\0';
}

/* stolen from
   https://code.google.com/p/1927code/source/browse/strrep.c
*/
/* ---------------------------------------------------------------------------
  Name       : replace - Search & replace a substring by another one.
  Creation   : Thierry Husson, Sept 2010
  Parameters :
	  str    : Big string where we search
	  oldstr : Substring we are looking for
	  newstr : Substring we want to replace with
	  count  : Optional pointer to int (input / output value). NULL to ignore.
			   Input:  Maximum replacements to be done. NULL or < 1 to do all.
			   Output: Number of replacements done or -1 if not enough memory.
  Returns    : Pointer to the new string or NULL if error.
  Notes      :
	 - Case sensitive - Otherwise, replace functions "strstr" by "strcasestr"
	 - Always allocates memory for the result.
--------------------------------------------------------------------------- */
char* replace(const char *str, const char *oldstr, const char *newstr, int *count)
{
	const char *tmp = str;
	char *result;
	int   found = 0;
	int   length, reslen;
	int   oldlen = (int)strlen(oldstr);
	int   newlen = (int)strlen(newstr);
	int   limit = (count != NULL && *count > 0) ? *count : -1;

	tmp = str;
	while ((tmp = strstr(tmp, oldstr)) != NULL && found != limit)
		found++, tmp += oldlen;

	length = (int)strlen(str) + found * (newlen - oldlen);
	if ((result = (char *)malloc(length + 1)) == NULL) {
		fprintf(stderr, "Not enough memory\n");
		found = -1;
	}
	else {
		tmp = str;
		limit = found; /* Countdown */
		reslen = 0; /* length of current result */
		/* Replace each old string found with new string  */
		while ((limit-- > 0) && (tmp = strstr(tmp, oldstr)) != NULL) {
			length = (int)(tmp - str); /* Number of chars to keep intouched */
			strncpy(result + reslen, str, length); /* Original part keeped */
			strcpy(result + (reslen += length), newstr); /* Insert new string */
			reslen += newlen;
			tmp += oldlen;
			str = tmp;
		}
		strcpy(result + reslen, str); /* Copies last part and ending nul char */
	}
	if (count != NULL) *count = found;
	return result;
}

double Fatorial(int value)
{
	double result;
	result = (value <= 1) ? value : (value * Fatorial(value - 1));
	return result;
}

bool VariableExists(const char *name, int *idx)
{
	int i;
	bool iFound = false;

	*idx = -1;

	for (i = 0; i < nVars; i++)
	{
		if (strcasecmp(name, varTable[i].name) == 0)
		{
			iFound = true;
			*idx = i;
			break;
		}
	}
	return iFound;
}

int adicionaVariavelDoUsuario(const char *name, double value)
{
	int iExists = false;
	int idx = 0;

	iExists = VariableExists(name, &idx);
	if (iExists)
	{
		varTable[idx].value = value;
		return 0;
	}

	if (nVars == MAX_VARIABLE - 1)
	{
		temErro = 1;
		fprintf(stderr, "Indice da variável fora dos limites!\n");
		return -1;
	}

	strncpy(varTable[nVars].name, name, MAX_NAME - 1);
	varTable[nVars].name[MAX_NAME - 1] = '\0';

	varTable[nVars].value = value;

	nVars++;

	return 0;
}

void setaValorDaVariavel(const char *name, double value)
{
	int idx = 0;
	if (!VariableExists(name, &idx))
	{
		fprintf(stderr, "Variavel \"%s\" nao localizada!\n", name);
		return;
	}

	varTable[idx].value = value;
}

double pegaValorDaVariavel(const char *name)
{
	int idx = 0;
	if (!VariableExists(name, &idx))
	{
		temErro = 1;
		fprintf(stderr, "Variavel \"%s\" nao localizada!\n", name);
		return -1;
	}

	return varTable[idx].value;
}

void incrementaValor(const char *name, double value)
{
	int idx = 0;
	if (!VariableExists(name, &idx))
	{
		temErro = 1;
		fprintf(stderr, "Variavel \"%s\" nao localizada!\n", name);
		return;
	}

	varTable[idx].value += value;
}

void decrementaValor(const char *name, double value)
{
	int idx = 0;
	if (!VariableExists(name, &idx))
	{
		//temErro = 1;
		fprintf(stderr, "Variavel \"%s\" nao localizada!\n", name);
		return;
	}

	varTable[idx].value -= value;
}

bool ConstantsExists(const char *name)
{
	int i;
	bool iFound = false;

	for (i = 0; i < nConsts; i++)
	{
		if (strcasecmp(name, constTable[i].name) == 0)
		{
			iFound = true;
			break;
		}
	}

	return iFound;
}

int adicionaConstanteDoUsuario(const char *name, double value)
{
	int iExists = false;

	iExists = ConstantsExists(name);
	if (iExists)
	{
		fprintf(stderr, "Constante ja existe!\n");
		return 0;
	}

	if (nConsts == MAX_CONSTS - 1)
	{
		temErro = 1;
		fprintf(stderr, "Constante indice fora dos limites!\n");
		return -1;
	}

	strncpy(constTable[nConsts].name, name, MAX_NAME - 1);
	constTable[nConsts].name[MAX_NAME - 1] = '\0';

	constTable[nConsts].value = value;

	nConsts++;

	return 0;
}

bool UserFunctionExists(char *fname, int *idx)
{
	int i;
	bool iFound = false;

	*idx = 0;
	for (i = 0; i < nUserFuncs; i++)
	{
		if (strcasecmp(fname, userFuncList[i].name) == 0)
		{
			*idx = i;
			iFound = true;
			break;
		}
	}
	return iFound;
}

bool GetUserFunction(char *fname)
{
	char *funcName;
	int i;
	char tmpStr[MAX_NAME];
	bool result = false;

	if (IsLetter(FParseText[iCurPos]))
	{
		for (i = 0; i < nUserFuncs; i++)
		{
			funcName = userFuncList[i].name;
			int iLen = (int)strlen(funcName);

			memset(tmpStr, 0, sizeof(tmpStr));
			SubString(FParseText, iCurPos, iLen, tmpStr);

			if (strcasecmp(tmpStr, funcName) == 0)
			{
				if (FParseText[iCurPos + iLen] == '(')
				{
					iCurPos += iLen;
					strcpy(fname, funcName);
					return true;
				}
			}
		}
	}
	return result;
}

bool GetFunction(TParserFunc *AValue)
{
	const char *_funcName = NULL;
	int i;
	char tmpStr[MAX_NAME];
	bool result = false;

	*AValue = pfArcTan;
	if (IsLetter(FParseText[iCurPos]))
	{
		for (i = 0; i < FuncTableSize; i++)
		{
			TParserFunc func = funcTables[i].func;
			_funcName = funcTables[i].name;
			int iLen = (int)strlen(_funcName);

			memset(tmpStr, 0, sizeof(tmpStr));
			SubString(FParseText, iCurPos, iLen, tmpStr);
			if (strcasecmp(tmpStr, _funcName) == 0)
			{
				*AValue = func;
				if (FParseText[iCurPos + iLen] == '(')
				{
					result = true;
					iCurPos += iLen;
					break;
				}
			}
		}
	}
	return result;
}

bool GetConst(double *AValue)
{
	int i;
	char tmpStr[MAX_NAME];
	bool result;

	result = false;
	*AValue = 0;
	if (IsLetter(FParseText[iCurPos]))
	{
		for (i = 0; i < nConsts; i++)
		{
			char *constName = constTable[i].name;
			int iLen = (int)strlen(constName);

			memset(tmpStr, 0, sizeof(tmpStr));
			SubString(FParseText, iCurPos, iLen, tmpStr);
			if (strcasecmp(tmpStr, constName) == 0)
			{
				*AValue = constTable[i].value;
				iCurPos += iLen;
				result = true;
				break;
			}
		}
	}
	return result;
}

bool GetVariable(double *AValue)
{
	int i;
	char tmpStr[MAX_NAME];
	bool result;

	result = false;
	*AValue = 0;
	if (IsLetter(FParseText[iCurPos]))
	{
		for (i = 0; i < nVars; i++)
		{
			char *varName = varTable[i].name;
			int iLen = (int)strlen(varName);

			memset(tmpStr, 0, sizeof(tmpStr));
			SubString(FParseText, iCurPos, iLen, tmpStr);
			if (strcasecmp(tmpStr, varName) == 0)
			{
				*AValue = varTable[i].value;
				iCurPos += iLen;
				result = true;
				break;
			}
		}
	}
	return result;

}

void registraFuncaoDoUsuario(char *funcName, UserFunction proc)
{
	int idx;
	if (strlen(funcName) > 0 && IsLetter(funcName[0]))
	{
		bool iExists = UserFunctionExists(funcName, &idx);
		if (!iExists)
		{
			strcpy(userFuncList[nUserFuncs].name, funcName);
			userFuncList[nUserFuncs].func = proc;
			nUserFuncs++;
		}
		else
		{
			temErro = 1;
			fprintf(stderr, "Erro do analisador!\n");
		}
	}
}

double Factor()
{
	double Value;
	char tmpStr[MAX_NAME];

	TParserFunc NoFunc;
	char UserFuncName[MAX_NAME];

	memset(tmpStr, 0, sizeof(tmpStr));
	memset(UserFuncName, 0, sizeof(UserFuncName));

	Value = 0;
	if (FParseText[iCurPos] == '(')
	{
		iCurPos++;
		Value = Expression();
		if (FParseText[iCurPos] != ')')
		{
			temErro = 1;
			fprintf(stderr, "Parenteses nao coicidem\n");
			return -1;
		}
		iCurPos++;
	}
	else
	{
		if (IsNumeric(FParseText[iCurPos]))
		{
			while (IsNumeric(FParseText[iCurPos]))
			{
				sprintf(tmpStr + strlen(tmpStr), "%c", FParseText[iCurPos]);
				iCurPos++;
			}
			Value = atof(tmpStr);
		}
		else
		{
			if (!GetConst(&Value))
				if (!GetVariable(&Value))
				{
					if (GetUserFunction(UserFuncName))
					{
						iCurPos++;

						int idx;
						bool iExists = UserFunctionExists(UserFuncName, &idx);
						if (iExists)
							Value = userFuncList[idx].func(Expression());
						if (FParseText[iCurPos] != ')')
						{
							temErro = 1;
							fprintf(stderr, "Faltou um ')'\n");
							return -1;
						}
						iCurPos++;
					}
					else
						if (GetFunction(&NoFunc))
						{
							iCurPos++;
							Value = Expression();
							switch (NoFunc) {
							case pfArcTan: Value = atan(Value); break;
							case pfCos: Value = cos(Value); break;
							case pfSin: Value = sin(Value); break;
							case pfTan:
								if (cos(Value) <= EPSILON)
								{
									temErro = 1;
									fprintf(stderr, "Impossivel calcular a tangente de 90!\n");
									return -1;
								}
								else
									Value = tan(Value);
								break;

							case pfAbs: Value = fabs(Value); break;
							case pfExp: Value = exp(Value); break;

							case pfLn:
								if (Value <= EPSILON)
								{
									temErro = 1;
									fprintf(stderr, "O valor de LN tem que ser menor que EPSILON!\n");
									return -1;
								}
								else
									Value = log(Value) / log(10);
								break;

							case pfLog:
								if (Value <= 0)
								{
									temErro = 1;
									fprintf(stderr, "Impossivel calcular o Log com valores menores ou iguais a 0!\n");
									return -1;
								}
								else
									Value = log(Value);
								break;

							case pfSqrt:
								if (Value < 0)
								{
									temErro = 1;
									fprintf(stderr, "Impossivel calcular raiz com numero menores que 0!\n");
									return -1;
								}
								else
									Value = sqrt(Value);
								break;

							case pfFact:
								if (Value < 0)
								{
									temErro = 1;
									fprintf(stderr, "Para calcular o fatorial o numero deve ser maior que 0!\n");
									return -1;
								}
								else
									Value = Fatorial((int)Value);
								break;

							case pfSqr:     Value = pow(Value, 2); break;
							case pfInt:     Value = (int)Value; break;
							case pfRound:   Value = round(Value); break;
							case pfFloor:   Value = floor(Value); break;
							case pfCeiling: Value = ceil(Value); break;
							case pfArcSin:  Value = (Value == 1) ? PI / 2 : asin(Value); break;
							case pfArcCos:  Value = (Value == 1) ? 0 : acos(Value); break;
							case pfSign: Value = (Value > 0) ? 1 : -1; break;
							}

							if (FParseText[iCurPos] != ')')
							{
								temErro = 1;
								fprintf(stderr, "Faltou um ')'\n");
								return -1;
							}

							iCurPos++;
						}
						else
						{
							temErro = 1;
							fprintf(stderr, "Erro de sintaxe!\n");
							return -1;
						}
				}
		}
	}

	return Value;
}

double Term()
{
	double Value;
	double tmpValue;

	Value = Factor();
	while (IsMulOp(FParseText[iCurPos]))
	{
		iCurPos++;
		switch (FParseText[iCurPos - 1])
		{
		case '*': Value = Value * Factor(); break;
		case '^': Value = pow(Value, Factor()); break;
		case '/':
			tmpValue = Factor();
			if (tmpValue <= EPSILON)
			{
				temErro = 1;
				fprintf(stderr, "Divisão por 0!\n");
				return -1;
			}
			Value = Value / Factor();
			break;
		}
	}
	return Value;
}

double Expression()
{
	double Value;

	Value = Term();
	while (IsAddOp(FParseText[iCurPos]))
	{
		iCurPos++;
		switch (FParseText[iCurPos - 1])
		{
		case '+': Value += Term(); break;
		case '-': Value -= Term(); break;
		}
	}

	if (!((FParseText[iCurPos] == '\0') ||
		FParseText[iCurPos] == ')' ||
		FParseText[iCurPos] == '='))
	{
		temErro = 1;
		return -1;
	}

	return Value;
}

void addSysConsts()
{
	static bool isAdded = false;

	if (isAdded) return;
	adicionaConstanteDoUsuario("PI", PI);
	adicionaConstanteDoUsuario("E", E);
	isAdded = true;
}

double calcula(const char *formula)
{
	double result = 0;
	char *ptr = NULL;

	char sFormula[MAX_FORMULA_LEN];
	char sTmp[MAX_FORMULA_LEN];
	char sParseText[MAX_FORMULA_LEN];

	temErro = 0;

	if (!formula || '\0' == *formula)
	{
		temErro = 1;
		fprintf(stderr, "Expressao vazia\n");
		return 0;
	}

	if (strlen(formula) >= MAX_FORMULA_LEN)
	{
		temErro = 1;
		fprintf(stderr, "Comprimento expressões eh maior que o maximo!\n");
		return 0;
	}

	memset(sFormula, 0, sizeof(sFormula));
	memset(sTmp, 0, sizeof(sTmp));
	memset(sParseText, 0, sizeof(sParseText));

	strcpy(sTmp, formula);
	StrTrim(sTmp, sFormula);
	if (0 == strlen(sFormula))
	{
		temErro = 1;
		fprintf(stderr, "Expressao vazia!\n");
		return 0;
	}

	int i = 0;
	int j = 0;
	int k = 0;
	memset(sTmp, 0, sizeof(sTmp));

	for (i = 0; i < strlen(sFormula); i++)
	{
		switch (sFormula[i])
		{
		case '(': j++; break;
		case ')': j--; break;
		}
		if (sFormula[i] > ' ')
			sTmp[k++] = sFormula[i];
	}

	if (j != 0)
	{
		temErro = 1;
		fprintf(stderr, "Os parenteses nao coincidem!\n");
		return 0;
	}

	sTmp[k] = '\0';

	if (sTmp[0] == '+' || sTmp[0] == '-')
	{
		sprintf(sParseText, "0%s", sTmp);
	}
	else
	{
		sprintf(sParseText, "%s", sTmp);
	}

	/* abs(-10.2) ==> abs(0-10.2) */
	memset(sTmp, 0, sizeof(sTmp));
	ptr = replace(sParseText, "(-", "(0-", NULL);
	if (ptr != NULL)
	{
		strcpy(sTmp, ptr);
		free(ptr);
	}

	/* abs(+10.2) ==> abs(0+10.2) */
	memset(sParseText, 0, sizeof(sTmp));
	ptr = replace(sTmp, "(+", "(0+", NULL);
	if (ptr != NULL)
	{
		strcpy(sParseText, ptr);
		free(ptr);
	}

	memset(FParseText, 0, sizeof(FParseText));
	strcpy(FParseText, sParseText);

	addSysConsts();

	iCurPos = 0;
	result = Expression();
	return result;
}

int TemErro()
{
	return temErro;
}
