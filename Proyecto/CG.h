/***************************************************************************
Code Generator
***************************************************************************/
/*-------------------------------------------------------------------------
Data Segment
-------------------------------------------------------------------------*/
int lbl = 0;
int data_offset = 0; /* Initial offset */
int data_location() /* Reserves a data location */
{
	return data_offset++;
}
/*-------------------------------------------------------------------------
Code Segment
-------------------------------------------------------------------------*/
int code_offset = 0; /* Initial offset */
int gen_label() /* Returns current offset */
{
	return code_offset;
}
int reserve_loc() /* Reserves a code location */
{
	return code_offset++;
}
/* Generates code at current location */
void gen_code( enum code_ops operation, int arg )
{ 
	code[code_offset].op = operation;
	code[code_offset++].arg = arg;
}
/* Generates code at a reserved location */
void back_patch( int addr, enum code_ops operation, int arg )
{
	code[addr].op = operation;
	code[addr].arg = arg;
}
/*-------------------------------------------------------------------------
Print Code to stdio
-------------------------------------------------------------------------*/
void print_icode()
{
	int i = 0;
	while (i < code_offset) {
		printf("%3ld: %-10s%4ld\n",i,op_name[(int) code[i].op], code[i].arg );
		i++;
	}
}

char *findsym(int offset)
{	symrec *ptr;
	for ( ptr = sym_table;ptr != (symrec *) 0; ptr = (symrec *)ptr->next ){
		if(ptr->offset==offset)
			return ptr->name;
	}
	return NULL;
}

void print_code()
{
	int i = 0,j;
	unsigned char *labels;
	symrec *ptr;
	char *symname;
	printf(".586\n");
	printf(".model 	flat,c\n");
	printf("printf	PROTO C :VARARG  ;\n");
	printf(".DATA	0x100\n");
	printf(".DATA\n");
	printf("fmtstr	DB	\"%%ld\",10,0\n");
	for ( ptr = sym_table;ptr != (symrec *) 0; ptr = (symrec *)ptr->next ){
		printf("%s	DD	0\n",ptr->name);
	}
	printf(".CODE\n");
	printf("MAIN	PROC\n");
	printf("	MOV	EAX,@DATA\n");
	printf("	MOV	EDS,EAX\n");
	printf("	MOV	EBP,ESP\n");
	/* Buscar líneas de saltos */
	labels = (char*)malloc(code_offset*2);
	while (i < code_offset) {
		j = code[i].arg;
		if(j>=code_offset)
			printf("ERR %d\n",j);
		if(code[i].op==GOTO || code[i].op==JMP_FALSE)
			labels[j] = 1;
		else
			labels[j] = 0;
		i++;
	}
	i = 0;
	while (i < code_offset) {
		//printf("%3ld: %-10s%4ld\n",i,op_name[(int) code[i].op], code[i].arg );
		if(labels[i])
			printf("L%d:\n",i);
		switch(code[i].op){
			case DATA:
				break;
			case HALT:
				printf("	PUSH	0\n");
				printf("	CALL	exit\n");
				break;
			case STORE:
				printf("	POP	EAX\n");
				symname = findsym(code[i].arg);
				printf("	LEA	EBX,OFFSET %s\n",symname);
				printf("	MOV	[EBX],EAX\n");
				//stack[ir.arg] = stack[top--]
				break;
			case JMP_FALSE:
				printf("	POP	EAX\n");
				printf("	CMP	EAX,0\n");
				printf("	JNE	@@%d\n",lbl);
				printf("	JMP	L%d\n",code[i].arg);
				printf("@@%d:\n",lbl++);
				break;
			case GOTO:
				printf("	JMP	L%d\n",code[i].arg);
				break;
			case LD_INT:
				printf("	MOV	EAX,%ld\n",code[i].arg);
				printf("	PUSH	EAX\n");
				break;
			case LD_VAR:
				symname = findsym(code[i].arg);
				printf("	MOV	EAX,[%s]\n",symname);
				printf("	PUSH	EAX\n");
				break;
			case READ_INT: 
				symname = findsym(code[i].arg);
				printf("	PUSH	OFFSET %s\n",symname);
				printf("	PUSH	OFFSET fmtstr\n");
				printf("	CALL	scanf\n");
				printf("	ADD	ESP,8\n");
				break;
			case WRITE_INT:
				printf("	PUSH	OFFSET fmtstr\n");
				printf("	CALL	printf\n");
				printf("	ADD	ESP,8\n");
				break;
			case LT: 
			    printf("	POP	EAX\n");
				printf("	POP	EBX\n");
				printf("	CMP	EAX,RBX\n");
				printf("	MOV	EAX,0\n");
				printf("	JNL	@@%d\n",lbl);
				printf("	MOV	EAX,1\n");
				printf("@@%d:\n",lbl++);
				printf("	PUSH	EAX\n");
				break;
			case EQ:
				printf("	POP	EAX\n");
				printf("	POP	EBX\n");
				printf("	CMP	EAX,RBX\n");
				printf("	MOV	EAX,0\n");
				printf("	JNE	@@%d\n",lbl);
				printf("	MOV	EAX,1\n");
				printf("@@%d:\n",lbl++);
				printf("	PUSH	EAX\n");
				break;
			case GT: 
				printf("	POP	EAX\n");
				printf("	POP	EBX\n");
				printf("	CMP	EAX,EBX\n");
				printf("	MOV	EAX,0\n");
				printf("	JNGE	@@%d\n",lbl);
				printf("	MOV	EAX,1\n");
				printf("@@%d:\n",lbl++);
				printf("	PUSH	EAX\n");
				//if ( stack[top-1] > stack[top] )
				//	stack[--top] = 1;
				//else 
				//	stack[--top] = 0;
				break;
			case ADD: 
				printf("	POP	EAX\n");
				printf("	POP	EBX\n");
				printf("	ADD	EAX,EBX\n");
				printf("	PUSH	EAX\n");
				//stack[top-1] = stack[top-1] + stack[top];
				//top--;
				break;
			case SUB: 
				printf("	POP	EAX\n");
				printf("	POP	EBX\n");
				printf("	SUB	EAX,EBX\n");
				printf("	PUSH	EAX\n");
				break;
			case MULT:
				printf("	POP	EAX\n");
				printf("	POP	EBX\n");
				printf("	MUL	EBX\n");
				printf("	PUSH	EAX\n");
				break;
			case DIV: 
				printf("	MOV	EDX,0\n");
				printf("	POP	ECX\n");
				printf("	POP	EAX\n");
				printf("	DIV	ECX\n");
				printf("	PUSH	EAX\n");
				break;
			default:
				break;
		}
		i++;
		/*printf("%d\n",i);*/
	}
	printf("	PUSH	0\n");
	printf("	CALL	exit\n");
	printf("	END	MAIN\n");
	/*printf("Freeing memory\n");*/
	free(labels);
}
/************************** End Code Generator **************************/
