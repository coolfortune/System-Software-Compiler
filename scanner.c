// Jeff Fortune

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"

/* list of reserved word names */
char *word[] = {"null", "begin", "call", "const", "do", "else", "end", "if", "odd", "procedure", "read", "then", "var", "while", "write"};

/* internal representation of reserved words */
int wsym[] = {nulsym, beginsym, callsym, constsym, dosym, elsesym, endsym, ifsym, oddsym, procsym, readsym, thensym, varsym, whilesym, writesym};


int convert(char c)
{
	return c - '0';
}

int searchToken(char *lex)
{
	for(int i = 0; i < norw; i++)
	{
		if (strcmp(lex, word[i]) == 0)
		{
			return wsym[i];
		}
	}

	return idsym;
}

int main(int argc, char **argv)
{

	// checks for correct number of arguments (program_name, source_code, output_file)
	if (argc < 3)
	{
		exit(1);
	}

	// Special Symbols
	int* ssym = calloc(256, sizeof(int));

	ssym['+'] = plussym;
	ssym['-'] = minussym;
	ssym['*'] = multsym;
	ssym['/'] = slashsym;
	ssym['('] = lparentsym;
	ssym[')'] = rparentsym;
	ssym['='] = eqlsym;
	ssym[','] = commasym;
	ssym['.'] = periodsym;
	ssym['#'] = neqsym;
	ssym['<'] = lessym;
	ssym['>'] = gtrsym;
	ssym['$'] = leqsym;
	ssym['%'] = geqsym;
	ssym[';'] = semicolonsym;

	// File initialization
	char *fileName = argv[1];
	char *out = argv[2];

	FILE *file = fopen(fileName, "r");
	FILE *out_file = fopen(out, "w");

	// Checks if file has been read successfully
	if (file == NULL || out_file == NULL)
	{
		fclose(file);
		fclose(out_file);
		exit(2);
	}

	// Allocated buffer to print out source code
	char* buf1 = calloc(imax, sizeof(char));
	fprintf(out_file, "Source Code:\n");
	while (!feof(file))
	{
		if(fgets(buf1, imax, file) != NULL)
		{
			fprintf(out_file, "%s", buf1);
			fflush(file);
		}
	}

	free(buf1);

	// Second buffer for Lexeme List
	char* buf2 = calloc(imax, sizeof(char));

	// Begin analyzer
	fprintf(out_file, "\n");
	fprintf(out_file, "Lexeme Table:\n");
	fprintf(out_file, "Lexeme\t\tToken Type\n");
	rewind(file);	// Rewinds file pointer to the beginning
	char c;

	lexer *lex_table = (lexer*) calloc(256, sizeof(lexer));

	int lp = 0;

	while (!feof(file))
	{
		c = fgetc(file);
		switch(c)
		{
			// Comments case
			// Ignores comments until closing symbols or returns error if symbol is missing
			case '/':
			{
				char d = c;
				c = fgetc(file);
				if(c == '*')
				{
					c = fgetc(file);
					while(1)
					{
						c = fgetc(file);
						if(c == '*')
						{
							while(c == '*')
								c = fgetc(file);
						}

						if(c == '/')
						{	
							break;
						}
						else
						{
							if(feof(file))
							{
								printf("ERROR: End comment \"*/\" not found!\n");
								exit(7);
							}
							else
								continue;
						}
					}
				}
				else
				{
					int token = ssym[d];

					if(token != 0)
					{
						fprintf(out_file, "%c\t\t\t%d\n", d, token);

						lex_table[lp].lexeme[0] = d;
						strcat(lex_table[lp].lexeme, "         ");

						lex_table[lp].token = token;
						lp++;

						char strbuf[3];
						sprintf(strbuf, "%d", token);
						strcat(buf2, strbuf);
						strcat(buf2, " ");
					}

					ungetc(c, file);
				}
			}
			// Values case
			// Reads digits until another symbol is encountered,
			// returns errors if value is too long or digit is followed
			// by a letter
			case '0' ... '9':
			{
				if(!isdigit(c))
					break;

				int value = convert(c);
				char* lex = (char*) calloc(nestmax, sizeof(char));
				int count = 1;
				while(isalnum(c))
				{
					if(isalpha(c))
					{
						printf("ERROR: Variable names cannot start with numbers!\n");
						exit(4);
					}
					if(count > nestmax)
					{
						printf("ERROR: Number too long!\n");
						exit(6);
					}

					strncat(lex, &c, 1);
					value = 10 * value + convert(c);
					c = fgetc(file);
					count++;
				}

				ungetc(c, file);

				fprintf(out_file, "%-11s\t%d\n", lex, numbersym);

				strcpy(lex_table[lp].lexeme, lex);
				lex_table[lp].token = numbersym;
				lp++;

				char strbuf[2];
				sprintf(strbuf, "%d", numbersym);
				strcat(buf2, strbuf);
				strcat(buf2, " ");
				strcat(buf2, lex);
				strcat(buf2, " ");

				free(lex);
				break;
			}
			// Identifier case
			// Reads until another symbol is encountered,
			// returns errors if id is too long
			case 'a' ... 'z':
			{
				char* lex = calloc(cmax, sizeof(char));
				int count = 1;
				while(isalpha(c) || isdigit(c))
				{
					if(count > cmax)
					{
						printf("ERROR: Name too long!\n");
						exit(6);
					}
					strncat(lex, &c, 1);
					c = fgetc(file);
					count++;
				}	

				ungetc(c, file);
				int token = searchToken(lex);

				fprintf(out_file, "%-11s\t%d\n", lex, token);
				
				strcpy(lex_table[lp].lexeme, lex);
				lex_table[lp].token = token;
				lp++;

				char strbuf[2];
				sprintf(strbuf, "%d", token);
				strcat(buf2, strbuf);
				strcat(buf2, " ");
				if(token == idsym)
				{
					strcat(buf2, lex);
					strcat(buf2, " ");
				}

				free(lex);
				break;
			}
			// Becomes case
			// Reads becomes symbol, returns an error if comma
			// is not followed by an equals sign
			case ':':
			{
				char *walrus = (char*) calloc(2, sizeof(char));
				walrus[0] = c;

				c = fgetc(file);

				if(c == '=')
				{
					walrus[1] = c;
					putc(walrus[0], out_file);
					putc(walrus[1], out_file);
					fprintf(out_file, "\t\t\t%d\n", becomessym);

					strcpy(lex_table[lp].lexeme, walrus);
					lex_table[lp].token = becomessym;
					lp++;

					char strbuf[3];
					sprintf(strbuf, "%d", becomessym);
					strcat(buf2, strbuf);
					strcat(buf2, " ");
				}
				else
				{
					printf("ERROR: Invalid Symbol Detected: %d\n", c);
					exit(3);
				}

				break;
			}
			// Less than group case
			case '<':
			{
				char comp[2];
				comp[0] = c;

				c = fgetc(file);

				int token = lessym;

				if(c == '=')
				{
					comp[1] = c;
					token = leqsym;
					fprintf(out_file, "%s\t\t\t%d\n", comp, token);

					strcpy(lex_table[lp].lexeme, comp);
					lex_table[lp].token = token;
					lp++;

				}
				else if(c == '>')
				{
					comp[1] = c;
					token = neqsym;
					fprintf(out_file, "%s\t\t\t%d\n", comp, token);

					strcpy(lex_table[lp].lexeme, comp);
					lex_table[lp].token = token;
					lp++;
				}
				else
				{
					ungetc(c, file);
					fprintf(out_file, "%s\t\t\t%d\n", comp, token);

					strcpy(lex_table[lp].lexeme, comp);
					lex_table[lp].token = token;
					lp++;
				}
					char strbuf[3];
					sprintf(strbuf, "%d", token);
					strcat(buf2, strbuf);
					strcat(buf2, " ");

				break;
			}
			// Greater than group case
			case '>':
			{
				char comp[2];
				comp[0] = c;

				c = fgetc(file);

				if(c == '=')
				{
					comp[1] = c;
					fprintf(out_file, "%s\t\t\t%d\n", comp, geqsym);

					strcpy(lex_table[lp].lexeme, comp);
					lex_table[lp].token = geqsym;
					lp++;
				}
				else
				{
					ungetc(c, file);
					fprintf(out_file, "%s\t\t\t%d\n", comp, gtrsym);

					strcpy(lex_table[lp].lexeme, comp);
					lex_table[lp].token = gtrsym;
					lp++;
				}

				break;
			}
			// Special case for EOF
			case -1:
			{
				break;
			}
			// All other cases (whitespace, special symbols, etc.)
			// Matches a special symbol with its respective token,
			// otherwise, ignores it if its whitespace.
			// Returns an error if any other symbol is read (sanity check)
			default:
			{
				if(isspace(c))
					break;

				int token = ssym[c];

				if(token != 0)
				{
					fprintf(out_file, "%c\t\t\t%d\n", c, token);

					lex_table[lp].lexeme[0] = c;
					strcat(lex_table[lp].lexeme, "         ");

					lex_table[lp].token = token;
					lp++;

					char strbuf[3];
					sprintf(strbuf, "%d", token);
					strcat(buf2, strbuf);
					strcat(buf2, " ");
				}
				else
				{
					printf("ERROR: Invalid Symbol Detected: %d\n", c);
					exit(4);
				}

				break;
			}

		}
	}
	// Prints Lexeme List
	for(int i = 0; i < lp; i++)
		printf("%d. %-11s\t\t%d\n", i, lex_table[i].lexeme, lex_table[i].token);

	parser(lex_table);
	free(lex_table);

	// Closes files
	fclose(file);
	fclose(out_file);
}
