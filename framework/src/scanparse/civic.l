%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "node_basic.h"
#include "str.h"
#include "free.h"
#include "civic.tab.h"
#include "globals.h"
#include "ctinfo.h"


#define FILTER(token) \
  global.col += yyleng;  \
  return( token);

#define LINEBUF_SIZE 256
//#define YY_NO_UNPUT

void yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
}

%}

%p 6000 /* set positions to 6000 */
%e 2000 /* set nodes to 1100 */
%x C_COMMENT
%option noyywrap
%%

"("                        { FILTER( BRACKET_L); }
")"                        { FILTER( BRACKET_R); }
"{"                        { FILTER( CBRACKET_L); }
"}"                        { FILTER( CBRACKET_R); }
","                        { FILTER( COMMA); }
";"                        { FILTER( SEMICOLON); }

"-"                        { FILTER( MINUS); }
"+"                        { FILTER( PLUS);  }
"*"                        { FILTER( MULT);  }
"/"                        { FILTER( DIV);  }
"%"                        { FILTER( MOD);  }

"<="                       { FILTER( LE);    }
"<"                        { FILTER( LT);    }
">="                       { FILTER( GE);    }
">"                        { FILTER( GT);    }
"=="                       { FILTER( EQ);    }
"!="                       { FILTER( NE);    }
"&&"                       { FILTER( AND);    }
"||"                       { FILTER( OR);    }
"!"                        { FILTER( NOT);   }

"="                        { FILTER( LET);    }

"true"                     { FILTER( TRUEVAL); }
"false"                    { FILTER( FALSEVAL); }

"extern"                   { FILTER( EXTERNKEY); }
"export"                   { FILTER( EXPORTKEY); }
"void"                     { FILTER( VOIDTYPE); }
"bool"                     { FILTER( BOOLTYPE); }
"int"                      { FILTER( INTTYPE); }
"float"                    { FILTER( FLOATTYPE); }
"if"                       { FILTER( IFCOND); }
"else"                     { FILTER( ELSECOND); }
"while"                    { FILTER( WHILELOOP); }
"do"                       { FILTER( DOLOOP); }
"for"                      { FILTER( FORLOOP); }
"return"                   { FILTER( RETURNSTMT); }




[A-Za-z][A-Za-z0-9_]*      { yylval.id = STRcpy(yytext);
                             FILTER( ID);
                           }

[0-9]+                     { yylval.cint=atoi(yytext);
                             FILTER( NUM);
                             printf("NUM FOUND: %s\n", yytext);
                           }

[0-9]+\.[0-9]+             { yylval.cint=atof(yytext);
                             FILTER( FLOAT);
                           }

\n.*                       { global.line += 1;
                             global.col = 0;
                             yyless(1);
                           }

[ \t]                      { global.col += yyleng;
                           }

"//".*\n                   { global.line += 1;
                             global.col = 0;
                           }

"/*"                       { BEGIN(C_COMMENT); }
<C_COMMENT>"*/"            { BEGIN(INITIAL); }
<C_COMMENT>\n              { global.line +=1;
                             global.col = 0;
                           }
<C_COMMENT>.               { global.col += yyleng; }


.                          { global.col += 1;
                             CTIerror(":%d: error: stray '%s' in program",
                              global.line, STRcpy(yytext));
                           }
