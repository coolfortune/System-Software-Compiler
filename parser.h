#ifndef PARSER_H
#define PARSER_H

#define MAX_STACK_HEIGHT 40
#define MAX_CODE_LENGTH 200
#define MAX_LEXI_LEVELS 3
#define MAX_SYMBOL_TABLE_LENGTH 200
#define MAX_LEXER_LENGTH 256

#define norw 15	/* number of reserved words */
#define imax 32767 /* maximum integer value */
#define cmax 11	/* maximum number of chars for idents */
#define nestmax 5  /* maximum depth of block nesting */
#define strmax 256 /* maximum length of strings */

typedef enum
{
	nulsym = 1,
	idsym,
	numbersym,
	plussym,
	minussym,
	multsym,
	slashsym,
	oddsym,
	eqlsym,
	neqsym,
	lessym,
	leqsym,
	gtrsym,
	geqsym,
	lparentsym,
	rparentsym,
	commasym,
	semicolonsym,
	periodsym,
	becomessym,
	beginsym,
	endsym,
	ifsym,
	thensym,
	whilesym,
	dosym,
	callsym,
	constsym,
	varsym,
	procsym,
	writesym,
	readsym,
	elsesym,
} token_type;

typedef struct
{
	char lexeme[11];
	int token;
} lexer;

typedef struct
{
	int kind;	  	// const = 1, var = 2, proc = 3
	char name[10]; 	// name up to 11 chars
	int val;	   	// number (ASCII value)
	int level;	 	// L  level
	int adr;	   	// M  address
} table;

typedef struct
{
	int op; // opcode
	int r;  // register
	int l;  // L
	int m;  // M
} instruction;


int convert(char r);

int searchToken(char *lex);

void error(int err);

void parser(lexer *lex_table);
void program(lexer *lex_table);
void block(lexer *lex_table);
void const_dec(lexer *lex_table);
void var_dec(lexer *lex_table);
void statement(lexer *lex_table);
void condition(lexer *lex_table);
void rel_op(lexer *lex_table);
void expression(lexer *lex_table);
void factor(lexer *lex_table);
void term(lexer *lex_table);
 
#endif
