#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_HEIGHT 40
#define MAX_CODE_LENGTH 200
#define MAX_LEXI_LEVELS 3

struct {
	int op; // opcode
	int r;  // register
	int l;  // L
	int m;  // M
} instruction;

int main()
{
	int* insRegister = calloc(8, sizeof(int));
	int* stack = calloc(MAX_STACK_HEIGHT, sizeof(int));

	int sp = 0;
	int	pc = 0;
	int	ir = 0;
	int bp = 1;

	void printStack(int* stack);
};



