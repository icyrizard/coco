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
    monop               cmonop;
    node               *node;
    type                ctype;
}

%error-verbose

%right LET
%left OR
%left AND
%nonassoc EQ NE
%nonassoc LT LE GT GE
%left PLUS MINUS
%left MULT "*" DIV "/"  MOD "%"
%right UNARYMINUS NOT
%right TYPECAST

%nonassoc IFX
%nonassoc ELSECOND

%token BRACKET_L BRACKET_R CBRACKET_L CBRACKET_R COMMA SEMICOLON ";"
%token TRUEVAL FALSEVAL

%token EXTERNKEY EXPORTKEY VOIDTYPE "void" BOOLTYPE INTTYPE FLOATTYPE
%token IFCOND WHILELOOP DOLOOP FORLOOP RETURNSTMT NOT

%token <cint> NUM
%token <cflt> FLOAT
%token <id> ID

%type <node> intval floatval boolval constant expr
%type <node> declaration program start
%type <node> fundec fundef funheader statementlist vardeclist
%type <node> vardec funbody statement returnstatement block
%type <node> globaldec globaldef exprlist

%type <ctype> basictype
%type <cmonop> monop
%type <node> param paramlist var


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

declaration: fundec
    {
        $$ = $1;
    }
    | fundef
    {
        $$ = $1;
    }
    | globaldec
    {
        $$ = $1;
    }
    | globaldef
    {
        $$ = $1;
    }
    ;

funheader: basictype var BRACKET_L paramlist BRACKET_R
    {
        $$ = TBmakeFunheader($1, $2, $4);

    }
    |  VOIDTYPE var BRACKET_L paramlist BRACKET_R
    {

        $$ = TBmakeFunheader(TYPE_void, $2, $4);
    }
    ;

fundec: EXTERNKEY funheader SEMICOLON
    {
        $$ = TBmakeFundec($2);
    }
    ;

fundef: EXPORTKEY funheader CBRACKET_L funbody CBRACKET_R
    {
        $$ = TBmakeFundef(TRUE, $2, $4);
    }
    | EXPORTKEY funheader CBRACKET_L CBRACKET_R
    {
        $$ = TBmakeFundef(TRUE, $2, NULL);
    }
    | funheader CBRACKET_L funbody CBRACKET_R
    {
        $$ = TBmakeFundef(FALSE, $1, $3);
    }
    | funheader CBRACKET_L CBRACKET_R
    {
        $$ = TBmakeFundef(FALSE, $1, NULL);
    }
    ;

globaldec: EXTERNKEY basictype var SEMICOLON
    {
        $$ = TBmakeGlobaldec( $2, $3);
    }
    ;

globaldef: EXPORTKEY basictype var LET expr SEMICOLON
    {
        $$ = TBmakeGlobaldef($2, TRUE, $3, $5);
    }
    | EXPORTKEY basictype var SEMICOLON
    {
        $$ = TBmakeGlobaldef($2, TRUE, $3, NULL);
    }
    | basictype var LET expr SEMICOLON
    {
        $$ = TBmakeGlobaldef($1, FALSE, $2, $4);
    }
    | basictype var SEMICOLON
    {
        $$ = TBmakeGlobaldef($1, FALSE, $2, NULL);
    }
    ;

funbody: vardeclist statementlist returnstatement
    {
        $$ = TBmakeFunbody($1, $2, $3);
    }
    | vardeclist statementlist
    {
        $$ = TBmakeFunbody($1, $2, NULL);
    }
    | vardeclist returnstatement
    {
        $$ = TBmakeFunbody($1, NULL, $2);
    }
    | vardeclist
    {
        $$ = TBmakeFunbody($1, NULL, NULL);
    }
    | statementlist returnstatement
    {
        $$ = TBmakeFunbody( NULL, $1, $2);
    }
    | statementlist
    {
        $$ = TBmakeFunbody( NULL, $1, NULL);
    }
    | returnstatement
    {
        $$ = TBmakeFunbody( NULL, NULL, $1);
    }
    ;

returnstatement: RETURNSTMT SEMICOLON
    {
        $$ = NULL;
    }
    | RETURNSTMT expr SEMICOLON
    {
        $$ = $2;
    }
    ;

vardeclist: vardec vardeclist
    {
        $$ = TBmakeVardeclist( $1, $2);
    }
    | vardec
    {
        $$ = TBmakeVardeclist( $1, NULL);
    }
    ;

vardec: basictype var LET expr SEMICOLON
    {
        $$ = TBmakeVardec( $1, $2, $4);
    }
    | basictype var SEMICOLON
    {
        $$ = TBmakeVardec( $1, $2, NULL);
    }
    ;

statementlist: statement statementlist
    {
        $$ = TBmakeStatementlist( $1, $2);
    }
    | statement
    {
        $$ = TBmakeStatementlist( $1, NULL);
    }
    ;
statement: var LET expr SEMICOLON
    {
        $$ = TBmakeAssign( $1, $3);
    }
    | var BRACKET_L exprlist BRACKET_R SEMICOLON
    {
        $$ = TBmakeFuncall( $1, $3);
    }
    |  IFCOND BRACKET_L expr BRACKET_R block    %prec IFX
    {
        $$ = TBmakeConditionif( $3, $5, NULL);
    }
    |  IFCOND BRACKET_L expr BRACKET_R block ELSECOND block
    {
        $$ = TBmakeConditionif( $3, $5, $7);
    }
    | WHILELOOP BRACKET_L expr BRACKET_R block
    {
        $$ = TBmakeWhileloop( $3, $5);
    }
    | DOLOOP block WHILELOOP BRACKET_L expr BRACKET_R SEMICOLON
    {
        $$ = TBmakeDowhileloop( $2, $5);
    }
    | FORLOOP BRACKET_L INTTYPE var LET expr COMMA expr COMMA expr BRACKET_R
    block
    {
        $$ = TBmakeForloop( TBmakeAssign($4, $6), $8, $10, $12);
    }
    | FORLOOP BRACKET_L INTTYPE var LET expr COMMA expr BRACKET_R
    block
    {
        $$ = TBmakeForloop( TBmakeAssign( $4, $6), $8, NULL, $10);
    }
    ;

block: CBRACKET_L statementlist CBRACKET_R
    {
        $$ = $2;
    }
    | statement
    {
        $$ = TBmakeStatementlist( $1, NULL);
    }
    ;

exprlist:  /* empty */
    {
        $$ = TBmakeExprlist( NULL, NULL);
    }
    | expr COMMA exprlist
    {
        $$ = TBmakeExprlist( $1, $3);
    }
    | expr
    {
        $$ = TBmakeExprlist( $1, NULL);
    }
    ;


expr: BRACKET_L expr BRACKET_R
    {
        $$ = $2;
    }                 /*  all binops  */

    | expr PLUS expr    { $$ = TBmakeBinop( BO_add, $1, $3); }
    | expr MINUS expr   { $$ = TBmakeBinop( BO_sub, $1, $3); }
    | expr MULT expr    { $$ = TBmakeBinop( BO_mul, $1, $3); }
    | expr DIV expr     { $$ = TBmakeBinop( BO_div, $1, $3); }
    | expr MOD expr     { $$ = TBmakeBinop( BO_mod, $1, $3); }
    | expr LE expr      { $$ = TBmakeBinop( BO_le, $1, $3); }
    | expr LT  expr     { $$ = TBmakeBinop( BO_ge, $1, $3); }
    | expr GE expr      { $$ = TBmakeBinop( BO_ge, $1, $3); }
    | expr GT expr      { $$ = TBmakeBinop( BO_gt, $1, $3); }
    | expr EQ expr      { $$ = TBmakeBinop( BO_eq, $1, $3); }
    | expr NE expr      { $$ = TBmakeBinop( BO_ne, $1, $3); }
    | expr OR expr      { $$ = TBmakeBinop( BO_or, $1, $3); }
    | expr AND expr     { $$ = TBmakeBinop( BO_and, $1, $3); }
                        /*  end of binops  */
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
    | var BRACKET_L exprlist BRACKET_R
    {
        $$ = TBmakeFuncall( $1, $3);
    }
    | var
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
    |
    {
        $$ = NULL;
    }
    ;

param: basictype var
    {
        $$ = TBmakeParam( $1, $2);
    }
    ;

var: ID
    {
        $$ = TBmakeVar( STRcpy( $1));
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
            global.line + 1, global.col, error);

  return( 0);
}

node *YYparseTree( void)
{
  DBUG_ENTER("YYparseTree");

  yyparse();

  DBUG_RETURN( parseresult);
}

