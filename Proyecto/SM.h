/***************************************************************************
Stack Machine
***************************************************************************/
#include "math.h"
/*=========================================================================
DECLARATIONS
=========================================================================*/
/* OPERATIONS: Internal Representation */
enum code_ops { HALT, STORE, JMP_FALSE, GOTO, DATA, LD_INT, LD_VAR, 
READ_INT, WRITE_INT,LT, LTE, EQ, NEQ, GT, GTE, ADD, SUB, MULT, DIV, 
MINUS, AND, OR, NOT,POT, SIN, COS, RAIZ};

/* OPERATIONS: External Representation */
char *op_name[] = {"halt", "store", "jmp_false", "goto", "data", "ld_int", "ld_var", "in_int", "out_int",
                   "lt",   "lte",  "eq", "neq", "gt", "gte", "add", "sub", "mult", "div", "minus" ,
                   "and" , "or" , "not", "pot", "sin", "cos", "raiz"};/*Declaracion de caracteres*/
                   
struct instruction
{
enum code_ops op;
int arg;
};
/* CODE Array */
struct instruction code[999];
/* RUN-TIME Stack */
int stack[999];
/*-------------------------------------------------------------------------
Registers
-------------------------------------------------------------------------*/
int pc = 0;
struct instruction ir;
int ar = 0;
int top = 0;
char ch;
/*=========================================================================
Fetch Execute Cycle
=========================================================================*/
void fetch_execute_cycle()
{ 
	do { /*printf( "PC = %3d IR.arg = %8d AR = %3d Top = %3d,%8d\n",
			pc, ir.arg, ar, top, stack[top]); */
		/* Fetch */
		ir = code[pc++];
		/* Execute */
		switch (ir.op) {
			case HALT : 
				printf( "halt\n" ); break;
			case READ_INT : 
				printf( "ENTRADA: " );
				scanf( "%ld", &stack[ar+ir.arg] ); break;
			case WRITE_INT : 
				printf( "SALIDA: %d\n", stack[top--] ); break;
			case STORE : 
				stack[ir.arg] = stack[top--]; break;
			case JMP_FALSE : 
				if ( stack[top--] == 0 )
					pc = ir.arg;
				break;
			case GOTO : 
				pc = ir.arg; break;
			case DATA : 
				top = top + ir.arg; break;
			case LD_INT : 
				stack[++top] = ir.arg; break;
			case LD_VAR : 
				stack[++top] = stack[ar+ir.arg]; break;
			case LT : 
				if ( stack[top-1] < stack[top] )
					stack[--top] = 1;
				else stack[--top] = 0;
				break;
			case LTE : 
				if ( stack[top-1] <= stack[top] )
					stack[--top] = 1;
				else stack[--top] = 0;
				break;
			case EQ : 
				if ( stack[top-1] == stack[top] )
					stack[--top] = 1;
				else stack[--top] = 0;
				break;
			case NEQ : 
				if ( stack[top-1] != stack[top] )
					stack[--top] = 1;
				else stack[--top] = 0;
				break;
			case GT : 
				if ( stack[top-1] > stack[top] )
					stack[--top] = 1;
				else stack[--top] = 0;
				break;
			case GTE : 
				if ( stack[top-1] >= stack[top] )
					stack[--top] = 1;
				else stack[--top] = 0;
				break;
			
			
			/* Cases de Operaciones Logicas*/	
			
			case AND : /*AND*/
				if ( stack[top-1] && stack[top] ) 
					stack[--top] = 1;
				else stack[--top] = 0;
				break;
			case OR : /*OR*/
				if ( stack[top-1] || stack[top] ) 
					stack[--top] = 1;
				else stack[--top] = 0;
				break;
			case NOT : /*NOT*/	
				 if(stack[top])
					stack[top]=0;
					else
						stack[top]=1;
				break;
			
			
			
			/* Cases de Operaciones Trigonometricas */	
			
			case SIN : /* Seno */ 
				stack[top-1]= sin(stack[top]);
				top--;
				break;
			case COS : /* Coseno */
				stack[top-1]= cos(stack[top]);
				top--;
				break;
			
			
			/*Operaciones Aritmeticas*/
			
             case POT : /*Potencia*/
				stack[top-1]= pow(stack[top-1], stack[top] );
				top--;
				break;
			
			case RAIZ : /* Raiz */
				stack[top-1]= sqrt(stack[top]);
				top--;
				break;

			case MULT : /* Multiplicacion */
				stack[top-1] = stack[top-1] * stack[top];
				top--;
				
				break;
			case DIV :  /* Division */
				stack[top-1] = stack[top-1] / stack[top];
				top--;
				
			case ADD :  /* Suma */
				stack[top-1] = stack[top-1] + stack[top];
				top--;
				break;
			case SUB : 	/* Resta */
				stack[top-1] = stack[top-1] - stack[top];
				top--;
				break;
			
				break;
			case MINUS : 
				stack[top] = -stack[top];
				break;
					
			default : 
				printf( "Invalid opcode: %ld\n", ir.op);
				break;
		}
	}
	while (ir.op != HALT);
}
/*************************** End Stack Machine **************************/
