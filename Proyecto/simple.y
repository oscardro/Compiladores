%{/*************************************************************************
Compiler for the Simple language
***************************************************************************/
/*=========================================================================
C Libraries, Symbol Table, Code Generator & other C code
=========================================================================*/
#include <stdio.h> /* For I/O */
#include <stdlib.h> /* For malloc here and in symbol table */
#include <string.h> /* For strcmp in symbol table */
#include "SM.h" /* Stack Machine */
#include "ST.h" /* Symbol Table */
#include "CG.h" /* Code Generator */
#include "math.h" 
#define YYDEBUG 1 /* For Debugging */
int errors; /* Error Count */
void install(char *sym_name);
int yylex(void);
void yyerror (char const *);
/*-------------------------------------------------------------------------
The following support backpatching
-------------------------------------------------------------------------*/
struct lbs /* Labels for data, if and while */
{
int for_goto;
int for_jmp_false;
};
struct lbs * newlblrec() /* Allocate space for the labels */
{
return (struct lbs *) malloc(sizeof(struct lbs));
}
/*-------------------------------------------------------------------------
Install identifier & check if previously defined.
-------------------------------------------------------------------------*/
void install ( char *sym_name )
{
	symrec *s;
	s = getsym (sym_name);
	if (s == 0)
		s = putsym (sym_name);
	else { 	errors++;
			printf( "%s is already defined\n", sym_name );
	}
}
/*-------------------------------------------------------------------------
If identifier is defined, generate code
-------------------------------------------------------------------------*/
void context_check( enum code_ops operation, char *sym_name )
{ 	symrec *identifier;
	identifier = getsym( sym_name );
	if ( identifier == 0 )
	{ 	errors++;
		printf( "%s", sym_name );
		printf( "%s\n", " is an undeclared identifier" );
	}
	else gen_code( operation, identifier->offset );
}
/*=========================================================================
SEMANTIC RECORDS
=========================================================================*/
%}
%union semrec /* The Semantic Records */
{
int intval; /* Integer values */
  char *id; /* Identifiers */
struct lbs *lbls; /* For backpatching */
}
/*=========================================================================
TOKENS
=========================================================================*/
%start program
%token <intval> NUMBER /* Simple integer */
%token <id> ID /* Simple identifier */
%token <lbls> IF WHILE /* For backpatching labels */
%token NOP THEN ELSE DO END 
%token SENO COSENO
%token INT INPUT PRINTF MAIN BEGINP VAR 
%token ASSGNOP
/*=========================================================================
OPERATOR PRECEDENCE
=========================================================================*/

%left '>' '=' '<' GTEOP LTEOP NEOP
%left '-' '+' 
%left '*' '/' 
%left RAIZZ POTENCIA        /*Operaciones Extras*/
%left '&' /*Operaciones Logicas*/
%left '|' /*Operaciones Logicas*/
%right NEG NEGACION 


/*=========================================================================
GRAMMAR RULES for the Simple language
=========================================================================*/
%%
program : MAIN '(' ')' '{' 
			header
		   { gen_code( DATA, data_location()); }
		    commands
         '}' { gen_code( HALT, 0 ); YYACCEPT; }
;
header: declarations
	;
declarations : /* empty */
    | INT id_seq ID ';' declarations{ install( $3 ); }
	;
id_seq : /* empty */
	| id_seq ID ',' { install( $2 ); }
	;
commands : /* empty */
	| commands command ';'
	;
command : NOP
	| INPUT ID               { context_check( READ_INT, $2 );                 }
	| PRINTF '(' expr ')'    { gen_code( WRITE_INT, 0 );                      }
	| ID ASSGNOP expr        { context_check( STORE, $1 );                    }
	| IF '(' expr ')'        { $1 = (struct lbs *) newlblrec();
	                 $1->for_jmp_false = reserve_loc();                       } 
	'{'commands'}'           { $1->for_goto = reserve_loc();                  }
         ELSE                { back_patch( $1->for_jmp_false,
						       JMP_FALSE,
						                                       gen_label() ); }
      
       '{'commands'}'        { back_patch( $1->for_goto, GOTO, gen_label() ); }
	| WHILE                  { 	$1 = (struct lbs *) newlblrec();
			$1->for_goto = gen_label(); }
	   '(' expr ')'          { $1->for_jmp_false = reserve_loc();             }
      '{'commands'}'         { gen_code( GOTO, $1->for_goto );
			back_patch( $1->for_jmp_false,
						JMP_FALSE,
						gen_label() );                                        }
	;
    /*Token Number*/
	expr : NUMBER 	  		 { gen_code( LD_INT, $1 );                        }
	
	/* Operaciones de forma descendente de mayor precendencia*/
	| ID 		  		            { context_check( LD_VAR, $1 ); }
	| expr '<' expr 	            { gen_code( LT,   0 );     }   /*Menor que*/
	| expr LTEOP expr 	            { gen_code( LTE,  0 );     }
	| expr '=' expr 	            { gen_code( EQ,   0 );     }     /*Igual*/
	| expr NEOP expr 	            { gen_code( NEQ,  0 );     }
	| expr '>' expr 	            { gen_code( GT,   0 );     }   /*Mayor que*/
	| expr GTEOP expr 	            { gen_code( GTE,  0 );     }
	

	/*Operaciones Aritmeticas*/
	| RAIZZ  '(' expr ')'	        { gen_code( RAIZ, 0 );     }       /*RAIZ*/
	| expr POTENCIA expr 	        { gen_code( POT,  0 );     }     /*POTENCIA*/
	| expr '+' expr 	            { gen_code( ADD,  0 );     }       /*Suma*/
	| expr '-' expr 	            { gen_code( SUB,  0 );     }      /*resta*/
	| expr '*' expr 	            { gen_code( MULT, 0 );     }  /*Multiplicacion*/
	| expr '/' expr 	            { gen_code( DIV,  0 );     }     /*Division*/
	| '-' expr %prec NEG            { gen_code( MINUS,0 );     }
	| '(' expr ')'
	;
	
	/*Operaciones trigonometricas*/
	| SENO   '(' expr ')'	        { gen_code( SIN,  0 );     }      /*SENO*/
	| COSENO '(' expr ')'	        { gen_code( COS,  0 );     }     /*COSENO*/
	
	/*Operaciones Logicas*/
	
	| NEGACION expr %prec NEG	    { gen_code( NOT,  0 );     }      /*NOT*/
	| expr '&' expr 	            { gen_code( AND,  0 );     }      /*AND*/
	| expr '|' expr 	            { gen_code( OR,   0 );     }       /*OR*/
	
%%
/*=========================================================================
MAIN
=========================================================================*/
void main( int argc, char *argv[] )

{ 	extern FILE *yyin;
	++argv; --argc;
	yyin = fopen( argv[0], "r" );
	/*yydebug = 1;*/
	errors = 0;
	puts("\n\n\n");
	printf("CODIGO TOKENIZADO:\n");
	puts("\n");
	yyparse ();	
	if ( errors == 0 )
	{ 	

        puts("\n");
        printf("\n\t\t*************************************************");
        printf("\n\t\t\t*COMPILACION FINALIZADA EXITOSAMENTE! *");
        printf("\n\t\t*************************************************");
        puts("\n\n\n\n\n");

        
		
		printf("\n************************************");
        printf("\n\t*CODIGO INTERMEDIO:*");
        printf("\n************************************");
		puts("\n");
		
		
		
		printf("LINEA	opcode	OPERANDO\n");
		print_icode ();
		
		
		
		printf("\n************************************");
        printf("\n\t*CODIGO ENSAMBLADOR:*");
        printf("\n************************************");
		puts("\n");
		print_code ();
		
		
		printf("\n\t\t*************************************************");
        printf("\n\t\t\t*EJECUTANDO CODIGO INTERMEDIO: *");
        printf("\n\t\t*************************************************");
		puts("\n");
		fetch_execute_cycle();
	}
	else
		printf ( "\n\nCOMPILACION FINALIZADA CON %ld ERRORES!\n\n",errors);
}
/*=========================================================================
YYERROR
=========================================================================*/

/*Funcion yyerror*/
void yyerror ( char const *s ) /* Called by yyparse on error */
{
	errors++;
	printf ("%s\n", s);
}
/**************************** End Grammar File ***************************/
