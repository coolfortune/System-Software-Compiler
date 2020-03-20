// Jeff Fortune

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"

char *errorMessages[27] = {
	"Use = instead of :=.\n",
	"= must be followed by a number.\n",
	"Identifier must be followed by =.\n",
	"const, var, must be followed by identifier.\n",
	"Semicolon or comma missing.\n",
	"Incorrect symbol after procedure declaration.\n",
	"Statement expected.\n",
	"Incorrect symbol after statement part in block.\n",
	"Period expected.\n",
	"Semicolon between statements missing.\n",
	"Undeclared identifier.\n",
	"Assignment to constant or procedure is not allowed.\n",
	"Assignment operator expected.\n",
	"Call must be followed by an identifier.\n",
	"Call of a constant or variable is meaningless.\n",
	"then expected.\n",
	"Semicolon or } expected.\n",
	"do expected.\n",
	"Incorrect symbol following statement.\n",
	"Relational operator expected.\n",
	"Expression must not contain a procedure identifier.\n",
	"Right parenthesis missing.\n",
	"The preceding factor cannot begin with this symbol.\n",
	"An expression cannot begin with this symbol.\n",
	"This number is too large.\n",
	"end expected.\n",
};

int err = 0; 	// Error Count

void error(int error)
{
	printf("ERROR CODE %d: %s", error, errorMessages[error]);
	err++;
}

int sp = 0; 	// Symbol Table Pointer

int cx = 0; 	// Code Index

int lp = 0; 	// Lex Table Pointer

table symbol_table[MAX_SYMBOL_TABLE_LENGTH];
instruction code[MAX_CODE_LENGTH];

void insert(lexer *lex_table, int kind)
{
	int offset = kind == 1 ? lp-3 : lp-1;
	int val = kind == 1 ? atoi(lex_table[lp-1].lexeme) : 0;

	symbol_table[sp].kind = kind;
	strcpy(symbol_table[sp].name, lex_table[offset].lexeme);
	symbol_table[sp].val = val;
	sp++;
}

void gen(int op, int r, int l, int m)
{
	if(cx > MAX_CODE_LENGTH)
		error(25);

	code[cx].op = op;
	code[cx].r = r;
	code[cx].l = l;
	code[cx].m = m;

	cx++;
}

int sym = 0;

void getsym(lexer *lex_table)
{
	if(lp > MAX_LEXER_LENGTH)
		error(26);

	sym = lex_table[lp].token;
	lp++;
}

void parser(lexer *lex_table)
{
	printf("Parsing...\n");

	program(lex_table);
}

void program(lexer *lex_table)
{
	getsym(lex_table);
	block(lex_table);

	if(sym != periodsym) error(8);

	if(err == 0)
	{
		printf("\nNo errors, prgram is syntactically correct.\n");
		printf("Interpreting...\n");
		//interpret(code);
	}
	else
		printf("\nErrors in PL/0 program\n");
}

void block(lexer *lex_table)
{
	if(sym == constsym)
	{
		while(sym == constsym)
			const_dec(lex_table);
	}
	if(sym == varsym)
	{
		while(sym == varsym)
			var_dec(lex_table);
	}

	statement(lex_table);
}

void const_dec(lexer *lex_table)
{
	getsym(lex_table);
	if(sym != idsym) error(3);

	getsym(lex_table);
	if(sym != eqlsym) error(2);

	getsym(lex_table);
	if(sym != numbersym) error(1);

	insert(lex_table, 1);

	getsym(lex_table);
	while(sym == commasym)
	{
		getsym(lex_table);
		if(sym != idsym) error(3);

		getsym(lex_table);
		if(sym != eqlsym) error(2);

		getsym(lex_table);
		if(sym != numbersym) error(1);

		insert(lex_table, 1);

		getsym(lex_table);
	}

	if(sym != semicolonsym) error(4);
	
	getsym(lex_table);
}

void var_dec(lexer *lex_table)
{
	getsym(lex_table);
	if(sym != idsym) error(3);

	insert(lex_table, 2);

	getsym(lex_table);
	while(sym == commasym)
	{
		getsym(lex_table);
		if(sym != idsym) error(3);

		insert(lex_table, 2);

		getsym(lex_table);
	}
	if(sym != semicolonsym) error(4);

	getsym(lex_table);
}

void statement(lexer *lex_table)
{
	switch(sym)
	{
		case idsym:
		{
			getsym(lex_table);
			if(sym != becomessym) error(12);

			getsym(lex_table);
			expression(lex_table);
			break;
		}
		case beginsym:
		{
			getsym(lex_table);
			statement(lex_table);

			while(sym == semicolonsym)
			{
				getsym(lex_table);
				statement(lex_table);
			}

			if(sym != endsym) error(25);
			getsym(lex_table);
			break;
		}
		case ifsym:
		{
			int ctemp;
	
			getsym(lex_table);
			condition(lex_table);

			if(sym != thensym) error(15);

			getsym(lex_table);
			ctemp = cx;
			gen(7, 0, 0, 0);
			statement(lex_table);
			code[ctemp].m = cx;
			break;
		}
		case whilesym:
		{
			getsym(lex_table);
			condition(lex_table);

			if(sym != dosym) error(17);

			getsym(lex_table);
			statement(lex_table);
			break;
		}
		case readsym:
		{
			getsym(lex_table);
			if(sym != idsym) error(2);
			getsym(lex_table);
			break;
		}
		case writesym:
		{
			getsym(lex_table);
			if(sym != idsym) error(2);
			getsym(lex_table);
			break;
		}
		default:
		{
			break;
		}
	}
}

void condition(lexer *lex_table)
{
	if(sym == oddsym)
	{
		getsym(lex_table);
		expression(lex_table);
	}
	else
	{
		expression(lex_table);

		rel_op(lex_table);

		expression(lex_table);
	}
}

void rel_op(lexer *lex_table)
{
	switch(sym)
	{
		case eqlsym:
		{
			getsym(lex_table);
			break;
		}
		case neqsym:
		{
			getsym(lex_table);
			break;
		}
		case lessym:
		{
			getsym(lex_table);
			break;
		}
		case leqsym:
		{
			getsym(lex_table);
			break;
		}
		case gtrsym:
		{
			getsym(lex_table);
			break;
		}
		case geqsym:
		{
			getsym(lex_table);
			break;
		}
		default:
		{
			error(2);
			break;
		}
	}
}

void expression(lexer *lex_table)
{
	int addop;

	if(sym == plussym || sym == minussym)
	{
		addop = sym;

		getsym(lex_table);
		term(lex_table);

		if(addop == minussym)
			gen(2, 0, 0, 1);
	}
	else
		term(lex_table);

	while(sym == plussym || sym == minussym)
	{
		addop = sym;

		getsym(lex_table);
		term(lex_table);

		if(addop == plussym)
			gen(2, 0, 0, 2);
		else
			gen(2, 0, 0, 3);
	}
}

void term(lexer *lex_table)
{
	int mulop;

	factor(lex_table);

	while(sym == multsym || sym == slashsym)
	{
		mulop = sym;

		getsym(lex_table);
		factor(lex_table);

		if(mulop == multsym)
			gen(2, 0, 0, 4);
		else
			gen(2, 0, 0, 5);
	}
}

void factor(lexer *lex_table)
{
	if(sym == idsym)
		getsym(lex_table);
	else if(sym == numbersym)
		getsym(lex_table);
	else if(sym == lparentsym)
	{
		getsym(lex_table);
		expression(lex_table);

		getsym(lex_table);
		if(sym != rparentsym) error(18);
		getsym(lex_table);

	}
	else error(17);
}
