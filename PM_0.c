#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_HEIGHT 40
#define MAX_CODE_LENGTH 200
#define MAX_LEXI_LEVELS 3

typedef struct{
	int op; // opcode
	int r;  // register
	int l;  // L
	int m;  // M
} Instruction;


void printInstructions(Instruction** insArray)
{
	for(int i = 0; i < MAX_CODE_LENGTH; i++)
	{
		if (insArray[i] == NULL)
			return;
		else
			printf("%d: %d %d %d\n", i, insArray[i]->r, insArray[i]->l, insArray[i]->m);
	}
	
		return;
}

Instruction** initInstructions(char* file)
{
    int op, r, l, m = 0;
	int i = 0;

    if (file == NULL)
        return NULL;

    FILE* inputFile = fopen(file, "r");

    if (inputFile == NULL)
        return NULL;

    Instruction** insArray = calloc(MAX_CODE_LENGTH, sizeof(Instruction*));

    while (fscanf(inputFile, "%d %d %d %d", &op, &r, &l, &m) != EOF)
    {
        insArray[i] = malloc(sizeof(Instruction*));
        insArray[i]->op = op;
        insArray[i]->r = r;
        insArray[i]->l = l;
        insArray[i]->m = m;
        i++;
    }

	fclose(inputFile);

    return insArray;
}

Instruction* insFetch(int* pc, Instruction** code)
{
	Instruction* instruction = code[*pc];

	if (instruction->op == 7)
	{
		*pc = instruction->m;
	}
	else
	{
		*pc += 1;
	}

	return instruction;
}

int base(int l, int* bp, int* stack)
{  
	int b1;
	b1 = *bp; 

	while (l > 0)
	{
		b1 = stack[b1 + 1];
		l--;
	}

	return b1;
}

void printStack(int* dataStack)
{
	int i = 0;

	for (i = 0; i < MAX_STACK_HEIGHT; i++)
	{
		printf("%d", dataStack[i]);
	}
}

void printCommand(int line, Instruction* instruction, char** op)
{
	int index = instruction->op;

	printf("%d %s %d %d %d", line, op[index], instruction->r,
			instruction->l, instruction->m);
}

void printExecutionHeader(int* pc, int* bp, int* sp, int* registers, int* stack)
{
	int i = 0;

	printf("\t\t\tpc\tbp\tsp\tRegisters\n");
	printf("Initial values\t\t0\t0\t0\t%d %d %d %d %d %d %d %d\n", registers[0], registers[1],
			registers[2], registers[3], registers[4], registers[5], registers[6], registers[7]);

	printf("Stack: ");
	for (i = 0; i < MAX_STACK_HEIGHT; i++)
	{
		printf("%d ", stack[i]);
	}

	printf("\n---------------------------------------------------------------------------\n");
}

void printExecution(int* pc, int* bp, int* sp, int* registers, int* dataStack, Instruction* commands,
					int breakLine)
{
	int j = 0;

	printf("\t\t\tpc\tbp\tsp\tRegisters\n");
	printf("\t\t\t\t%d 	%d 	%d 	%d %d %d %d %d %d %d %d\n", *pc, *bp, *sp, registers[0], registers[1],
		registers[2], registers[3], registers[4], registers[5], registers[6], registers[7]);

	printf("Stack: ");
	for (j = 0; j < (breakLine * 6); j++)
	{
		if (j % 6 == 0 && j != 0)
		{
			printf("| ");
		}

		printf("%d ", dataStack[j]);
	}

	printf("\n---------------------------------------------------------------------------\n");
}


int main(int argc, char** argv)
{
	if (argc < 1)
		return 1;

	char* operators[24] = {"LIT", "RTN", "LOD", "STO", "CAL", "INC", "JMP", "JPC",
								"SIO", "SIO", "SIO", "NEG", "ADD", "SUB", "MUL", "DIV",
								"ODD", "MOD", "NEQ", "LSS", "LEQ", "GTR", "GEQ"};

	char* input = argv[1];

	int* registers = calloc(8, sizeof(int));
	int* stack = calloc(MAX_STACK_HEIGHT, sizeof(int));
	Instruction** insArray = initInstructions(input);

	int sp = 0;
	int	pc = 0;
	int	ir = 0;
	int bp = 1;

	int halt = 0;

	int breakLine = 1;

	int line = 0;

	printInstructions(insArray);
	printExecutionHeader(&pc, &bp, &sp, registers, stack);

	// Run Program
	while(halt == 0)
	{	
		Instruction* curInstruction = insFetch(&pc, insArray);

		switch(curInstruction->op)
		{
			// LIT
			case 1:
			{
				registers[curInstruction->r] = curInstruction->m;
			}
			// RTN
			case 2:
			{
				sp = bp - 1;
				bp = stack[sp + 2];
				pc = stack[sp + 3];

				breakLine -= 1;
			}
			// LOD
			case 3:
			{
				registers[curInstruction->r] = stack[(base(curInstruction->l, &bp, stack) + curInstruction->m)-1];
			}
			// STO
			case 4:
			{
				stack[(base(curInstruction->l, &bp, stack) + curInstruction->m)-1] = registers[curInstruction->r];
			}
			// CAL
			case 5:
			{
				stack[sp] = 0;
				stack[sp + 1] = base(curInstruction->l, &bp, stack);
				stack[sp + 2] = bp;
				stack[sp + 3] = pc;
				bp = sp + 1;
				pc = curInstruction->m;
				breakLine += 1;
			}
			// INC
			case 6:
			{
				sp = sp + curInstruction->m;
			}
			// JMP
			case 7:
			{
				pc = curInstruction->m;
			}
			// JPC
			case 8:
			{
				if (registers[curInstruction->r] == 0)
				{
					pc = curInstruction->m;
				}
			}
			// SIO
			case 9:
			case 10:
		    case 11:
			{
				switch(curInstruction->m)
				{
					case 1:
						printStack(stack);
						break;
					case 2: 
						break;
					case 3:
						halt = 1;
						break;
				}
			}
			// NEG
			case 12:
			{
				registers[curInstruction->r] = registers[curInstruction->r] * -1;
			}
			// ADD
			case 13:
			{
				registers[curInstruction->r] = registers[curInstruction->l] + registers[curInstruction->m];
			}
			// SUB
			case 14:
			{
				registers[curInstruction->r] = registers[curInstruction->l] - registers[curInstruction->m];
			}
			// MUL
			case 15:
			{
				registers[curInstruction->r] = registers[curInstruction->l] * registers[curInstruction->m];
			}
			// DIV
			case 16:
			{
				registers[curInstruction->r] = registers[curInstruction->l] / (registers[curInstruction->m] == 0) ? 1 : registers[curInstruction->m];
			}
			// ODD
			case 17:
			{
				registers[curInstruction->r] = registers[curInstruction->r] % 2;
			}
			// MOD
			case 18:
			{
				registers[curInstruction->r] = registers[curInstruction->l] % (registers[curInstruction->m] == 0) ? 1 : registers[curInstruction->m];
			}
			// EQL
			case 19:
			{
				if (registers[curInstruction->l] == registers[curInstruction->m])
				{
					registers[curInstruction->r] = 1;
				}
				else
				{
					registers[curInstruction->r] = 0;
				}
			}
			// NEQ
			case 20:
			{
				if (registers[curInstruction->l] != registers[curInstruction->m])
				{
					registers[curInstruction->r] = 1;
				}
				else
				{
					registers[curInstruction->r] = 0;
				}
			}
			// LSS
			case 21:
			{
				if (registers[curInstruction->l] < registers[curInstruction->m])
				{
					registers[curInstruction->r] = 1;
				}
				else
				{
					registers[curInstruction->r] = 0;
				}
			}
			// LEQ
			case 22:
			{
				if (registers[curInstruction->l] <= registers[curInstruction->m])
				{
					registers[curInstruction->r] = 1;
				}
				else
				{
					registers[curInstruction->r] = 0;
				}
			}
			// GTR
			case 23:
			{
				if (registers[curInstruction->l] > registers[curInstruction->m])
				{
					registers[curInstruction->r] = 1;
				}
				else
				{
					registers[curInstruction->r] = 0;
				}
			}
			//GEQ
			case 24:
			{
				if (registers[curInstruction->l] >= registers[curInstruction->m])
				{
					registers[curInstruction->r] = 1;
				}
				else
				{
					registers[curInstruction->r] = 0;
				}
			}
		}

		printCommand(line+1, curInstruction, operators);
		printExecution(&pc, &bp, &sp, registers, stack, curInstruction,
					breakLine);

		line = pc - 1;
	}

}

