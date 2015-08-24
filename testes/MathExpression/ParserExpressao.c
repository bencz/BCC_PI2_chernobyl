#include <stdio.h>
#include <math.h>
#include "ParserExpressao.h"

/*
{ BNF:
  Expression::=<Term> [<AddOp> <Term>]*
  <Term>    ::=<Factor> [<MulOp> <Factor> ]*
  <Factor>  ::=(<Expression>) | <Numeric> | <Constant> | UserFunc | PreDefinedFunc
}
*/
