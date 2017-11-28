/***************************************************************************
Scanner for the Simple language
***************************************************************************/
%{
/*=========================================================================
C-libraries and Token definitions
=========================================================================*/
#include <string.h> /* for strdup */
#include <stdlib.h> /* for atoi */
#include "simple.tab.h" /* for token definitions and yylval */
%}
/*=========================================================================
TOKEN Definitions
=========================================================================*/
DIGIT [0-9]
ID [a-z][a-z0-9]*
/*=========================================================================
REGULAR EXPRESSIONS defining the tokens for the Simple language
=========================================================================*/
%%
"<="   { printf("%s ",yytext);return(LTEOP);     }
">="   { printf("%s ",yytext);return(GTEOP);     }
"^"    { printf("%s ",yytext);return(POTENCIA);  }
"sin"  { printf("%s ",yytext);return(SENO);      }/*Impresion de nuestro valor Seno*/
"cos"  { printf("%s ",yytext);return(COSENO);    }/*Impresion de nuestro valor Coseno*/
"raiz" { printf("%s ",yytext);return(RAIZZ);     }/*Impresion de nuestro valor Raiz*/
"!="   { printf("%s ",yytext);return(NEOP);      }
"="    { printf("%s ",yytext);return(ASSGNOP);   }

{DIGIT}+ { printf("NUMBER(%s) ",yytext);yylval.intval = atoi( yytext );return(NUMBER); }
do { printf("DO ",yytext);return(DO);            }
else { printf("ELSE ");return(ELSE);             }
end { printf("END ");return(END);                }
if { printf("IF ");return(IF);                 }
not { printf("NOT");return(NEGACION);            }
begin { printf("BEGIN ");return(BEGINP);         }
int { printf("INT ");return(INT);                }
main { printf("MAIN \n",yytext);return(MAIN);    }
input { printf("INPUT ",yytext);return(INPUT);   }
nop { printf("NOP ");return(NOP);                }
then { printf("THEN ");return(THEN);             }
while { printf("WHILE ");return(WHILE); }
printf { printf("PRINTF ");return(PRINTF); }
var   { printf("VAR ");return(VAR);     }
{ID} { printf("ID(%s) ",yytext); yylval.id = (char *) strdup(yytext);return(ID); }
[ \t\n\r]+ /* eat up whitespace */{ }
. { printf("%c \n",*yytext);return(yytext[0]); }
%%

int yywrap(void){ return 0;}
/************************** End Scanner File */
