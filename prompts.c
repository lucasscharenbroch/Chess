#include "chess.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef BOOL
#define BOOL 0
#define INT 1
#define STRING 2
#endif

#define MAX_STRING_SIZE 2000

/*
 * reads characters from input until a 'y', 'Y', 'n' or 'N'
 * is encountered. the rest of the line is read, and
 * the corresponding result is returned.
 *
 * returns:
 * int, the boolean value that is read (TRUE or FALSE)
 */
int getBoolFromInput() {
	int boolValue;
	char c;

	while(TRUE){
		c = tolower(getchar());
		if(c == 'y') {
			boolValue = TRUE;
			break;
		} else if(c == 'n') {
			boolValue = FALSE;
			break;
		}	
	}

	/* read the rest of the line */

	while(c != '\n')
		c = getchar();

	return boolValue;
}

/*
 * reads input until a digit is encontered. Digits are then copied into intVal 
 * until a non-digit character is reached. The rest of the line is read, and 
 * intVal is returned.
 *
 * returns:
 * int, the value read from input
 */
int getIntFromInput() {
	int intValue = 0; 	
	int valueWasRead = FALSE;
	char c;

	while(TRUE) {
		c = getchar();

		if(isdigit(c)) {
			valueWasRead = TRUE;
			intValue *= 10;
			intValue += c - '0';
		} else {
			if(valueWasRead)
				break;
		}
	}

	/* read the rest of the line */

	while(c != '\n')
		c = getchar();

	return intValue;
}

/*
 * reads characters from input into string until whitespace (' ' '\t' or '\n')
 * is reached, or string reaches maxSize-1. 
 *
 * recieves:
 * string, an array of chars to copy the input into
 * maxSize, the length of string. 
 */
void getStringFromInput(char *string, int maxSize) {
	char c;
	int i = 0; /* index in string */

	while(!isspace(c = getchar()) && i < maxSize-1) {
		string[i++] = c;		
	}

	/* truncate string */
	string[i] = '\0';
}


/*
 * prints promptString and returns a pointer to
 * the parsed respose.
 *
 * recieves:
 * char *promptString: string to print
 * int type: BOOL(0), INT(1), or STRING(2): type of response to parse and return
 *
 * returns:
 * void pointer to the parsed response, in the type specified
 * NULL is returned when the type is invalid.
 */
void *prompt(char *promptString, int type) {
	printf("%s", promptString);	

	switch(type) {
		case BOOL:
			{
				int *result = malloc(sizeof(int));
				*result = getBoolFromInput();
				return (void *)result;
			}
		case INT:
			{
				int *result = malloc(sizeof(int));
				*result = getIntFromInput(result);
				return (void *)result;
			}
		case STRING:
			{
				char *result = malloc(sizeof(char) * MAX_STRING_SIZE);
				getStringFromInput(result, MAX_STRING_SIZE);
				return (void *)result;
			}
		default:
			return NULL;	
	}
}
