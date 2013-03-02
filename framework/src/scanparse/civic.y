%{


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include "types.h"
#include "tree_basic.h"
#include "str.h"
#include "dbug.h"
#include "ctinfo.h"
#include "free.h"
#include "globals.h"
#include "mytypes.h"

int yydebug=1;

static node *parseresult = NULL;
extern int yylex();
static int yyerror( char *errname);

%}

%union {
    nodetype            nodetype;
    char               *id;
    int                 cint;
    float               cflt;
    binop               cbinop;
    monop               cmonop;
    node               *node;
    type                ctype;
}

%right LET
%left OR
%left AND
%nonassoc EQ NE
%nonassoc LT LE GT GE
%left PLUS MINUS
%left MULT DIV MOD
%right UNARYMINUS NOT
%right TYPECAST
%nonassoc REDUCEBINOP   //TODO Is this at the right spot?


%token BRACKET_L BRACKET_R COMMA SEMICOLON
%token TRUEVAL FALSEVAL

%token EXTERNKEY EXPORTKEY VOIDTYPE BOOLTYPE INTTYPE FLOATTYPE
%token IFCOND ELSECOND WHILELOOP DOLOOP FORLOOP RETURNSTMT NOT

%token <cint> NUM
%token <cflt> FLOAT
%token <id> ID

%type <node> intval floatval boolval constant expr
%type <node> declaration program start
%type <cbinop> binop

%type <ctype> basictype
%type <cmonop> monop
%type <node> param paramlist varlet


%start start

%%

start: program
        {
            parseresult = $1;
        }
        ;

program: declaration program
        {
            $$ = TBmakeProgram( $1, $2);
        }
      | declaration
        {
            $$ = TBmakeProgram( $1, NULL);
        }
        ;

// dummy semicolon for shift/reduce conflict
// processing "a - b" we want:
// "a - b" -> .. -> expr binop expr -> expr -> declaration -> program

// but "a - b" could also be seen as two parts  "a" and "- b" on seperate lines
// "a" -> varlet -> expr -> declaration program
// "- b" -> .. -> monop expr -> expr -> declaration
declaration: expr SEMICOLON
        {
            $$ = $1;
        }
        ;

expr: BRACKET_L expr BRACKET_R
        {
            $$ = $2;
        }
        | expr binop expr             %prec REDUCEBINOP
        {
            $$ = TBmakeBinop( $2, $1, $3);
        }
        | expr MINUS expr
        {
            $$ = TBmakeBinop( BO_sub, $1, $3);
        }
        | MINUS expr                  %prec UNARYMINUS
        {
            $$ = TBmakeMonop( MO_neg, $2);
        }
        | monop expr                  %prec UNARYMINUS
        {
            $$ = TBmakeMonop( $1, $2);
        }
        | BRACKET_L basictype BRACKET_R expr    %prec TYPECAST
        {
            $$ = TBmakeCast( $2, $4);
        }
        | varlet BRACKET_L paramlist BRACKET_R
        {
            $$ = TBmakeFuncall( $1, $3);
        }
        | varlet
        {
            $$ = $1;
        }
        | constant
        {
            $$ = $1;
        }
        ;

paramlist: param COMMA paramlist
        {
            $$ = TBmakeParamlist( $1, $3);
        }
        | param
        {
            $$ = TBmakeParamlist( $1, NULL);
        }
        ;

param: basictype varlet
        {
            $$ = TBmakeParam( $1, $2);
        }
        ;

varlet: ID
        {
            $$ = TBmakeVarlet( STRcpy( $1));
        }
        ;

constant: floatval
        {
            $$ = $1;
        }
        | intval
        {
            $$ = $1;
        }
        | boolval
        {
            $$ = $1;
        }
        ;

floatval: FLOAT
        {
             $$ = TBmakeFloat( $1);
        }
        ;

intval: NUM
        {
            $$ = TBmakeNum( $1);
        }
        ;

boolval: TRUEVAL
        {
            $$ = TBmakeBool( TRUE);
        }
        | FALSEVAL
        {
            $$ = TBmakeBool( FALSE);
        }
        ;

binop: PLUS      { $$ = BO_add; }
     | MULT      { $$ = BO_mul; }
     | DIV       { $$ = BO_div; }
     | MOD       { $$ = BO_mod; }
     | LE        { $$ = BO_le; }
     | LT        { $$ = BO_lt; }
     | GE        { $$ = BO_ge; }
     | GT        { $$ = BO_gt; }
     | EQ        { $$ = BO_eq; }
     | OR        { $$ = BO_or; }
     | AND       { $$ = BO_and; }
     ;

monop: NOT       { $$ = MO_not; }
     ;

basictype: BOOLTYPE    { $$ = TYPE_bool; }
     | INTTYPE         { $$ = TYPE_int; }
     | FLOATTYPE       { $$ = TYPE_float; }
     ;


%%

static int yyerror( char *error)
{
  CTIabort( "line %d, col %d\nError parsing source code: %s\n",
            global.line, global.col, error);

  return( 0);
}

node *YYparseTree( void)
{
  DBUG_ENTER("YYparseTree");

  yyparse();

  DBUG_RETURN( parseresult);
}

