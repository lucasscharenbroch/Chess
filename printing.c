#include "chess.h"

#define WHITE_GREEN L"\x1b[37;42m"
#define WHITE_BLACK L"\x1b[37;40m"

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

/*
 * puts the console into virtual output mode
 */
int setVirtualMode() {
	HANDLE hStdout;
	DWORD dwMode = 0;
	if (!GetConsoleMode(hStdout, &dwMode)) {
		return 1;
	}
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

	SetConsoleMode(hStdout, dwMode);

	return 0;
}

/*
 * calls setVirtualMode
 */
void consoleSetup() {
	setVirtualMode();
}

/*
 * sets the color of the console output
 */
void setColor(const wchar_t * color) {
	wprintf(color);
}

/*
 * prints a piece from gamePtr->board (w/ highlight)
 */
void printPiece(GameState *gamePtr, int row, int col) {
	if(gamePtr->highlighted[row][col]) {
		setColor(WHITE_GREEN);
		printf("%c", gamePtr->board[row][col]);
		setColor(WHITE_BLACK);
	} else {
		printf("%c", gamePtr->board[row][col]);
	}
	printf(" ");
}

/*
 * prints the board from black's perspective
 */
void printReversedBoard(GameState *gamePtr) {
	static char colNames[] = "hgfedcba";

	/* print column names */
	printf("  ( ");
	for(int i = 0; i < 8; i++) {
		printf("%c ", colNames[i]);
	}	
	printf(")\n");

	/* print the rest of the board */
	for(int row = 7; row >= 0; row--) {
		printf("(%c) ", rowToLetter(row));
		for(int col = 7; col >= 0; col--) {
			printPiece(gamePtr, row, col);
		}
		printf("\n");
	}
}

/*
 * prints the board
 */
void printBoard(GameState *gamePtr) {
	static char colNames[] = "abcdefgh";

	/* print column names */
	printf("  ( ");
	for(int i = 0; i < 8; i++) {
		printf("%c ", colNames[i]);
	}	
	printf(")\n");

	/* print the rest of the board */
	for(int row = 0; row < 8; row++) {
		printf("(%c) ", rowToLetter(row));
		for(int col = 0; col < 8; col++) {
			printPiece(gamePtr, row, col);
		}
		printf("\n");
	}
}

/*
 * prints the board, player stats, and turn status
 */
void printGameInfo(GameState *gamePtr) {
	system("cls");
	

	if(gamePtr->turn == BLACK && gamePtr->printInvertedBoard) {
		/* print white stats */
		printf("White\n");
		printf("%s (%d)\n\n", gamePtr->whiteCapturedPieces, gamePtr->whiteScore);
		/* board */
		printReversedBoard(gamePtr);
		/* print black stats */
		printf("\nBlack\n");
		printf("%s (%d)\n\n", gamePtr->blackCapturedPieces, gamePtr->blackScore);
	}else{
		/* print black stats */
		printf("Black\n");
		printf("%s (%d)\n\n", gamePtr->blackCapturedPieces, gamePtr->blackScore);
		/* board */
		printBoard(gamePtr);
		/* print white stats */
		printf("\nWhite\n");
		printf("%s (%d)\n\n", gamePtr->whiteCapturedPieces, gamePtr->whiteScore);
	}

	printf("%s's Turn.\n\n", (gamePtr->turn == WHITE) ? "White" : "Black");
}

/*
 * sets every element of gamePtr->highlighted to FALSE
 */
void clearHighlights(GameState *gamePtr) {
	for(int r = 0; r < 8; r++) {
		for(int c = 0; c < 8; c++) {
			gamePtr->highlighted[r][c] = FALSE;
		}
	}
}

/*
 * sets gamePtr->highlighted based on the possible moves of the
 * piece at the coordinate specificed in playerAction.
 */
void setHighlights(GameState *gamePtr, char *coord) {
	clearHighlights(gamePtr);

	char **legalMoves = malloc(30 * sizeof(char *));
	int numMoves;

	int col =  letterToCol(coord[0]);
	int row = letterToRow(coord[1]);
	numMoves = getPieceLegalMoves(gamePtr, legalMoves, row, col); 

	for(int i = 0; i < numMoves; i++) {
		col = letterToCol(legalMoves[i][2]);
		row = letterToRow(legalMoves[i][3]);	

		gamePtr->highlighted[row][col] = TRUE;			
	}
	freeStringArray(legalMoves, numMoves);
}

/*
 * asks player to ender a move or a coordinate
 * if a move (coordiante pair) is entered, it is returned.
 * if a coordinate is entered, legal moves that originate
 * from that coordinate are found, and gamePtr->highlighted
 * is set accordingly. The game info is printed again,
 * and gamePtr->highlighted is reset.
 */
void promptForMove(GameState *gamePtr, char *playerMovePtr) {
	char promptString[] = "To make a move, enter a coordinate pair (E.G. a2b4) \n\
To make highlight possible moves of a piece, enter \n\
the coordinate of that piece.\n";

	char *userResponse;
	while(TRUE) {
		userResponse = prompt(promptString, STRING);
		if(strlen(userResponse) == 2) {
			int char1Valid = userResponse[0] >= 'a' && userResponse[0] <= 'h';
			int char2Valid = userResponse[1] >= '1' && userResponse[1] <= '8'; 
			if(char1Valid && char2Valid) {
				setHighlights(gamePtr, userResponse);	
				printGameInfo(gamePtr);
				clearHighlights(gamePtr);
			}
		}else if(strlen(userResponse) == 4) {
			/* castle */
			if(strcmp(KING_SIDE_CASTLE, userResponse) == 0 ||
					strcmp(QUEEN_SIDE_CASTLE, userResponse) == 0) {
				strcpy(playerMovePtr, userResponse);
				free(userResponse);
				return;
			}
			int char1Valid = userResponse[0] >= 'a' && userResponse[0] <= 'h';
			int char2Valid = userResponse[1] >= '1' && userResponse[1] <= '8'; 
			int char3Valid = userResponse[2] >= 'a' && userResponse[2] <= 'h';
			int char4Valid = userResponse[3] >= '1' && userResponse[3] <= '8'; 
			if(char1Valid && char2Valid && char3Valid && char4Valid) {
				strcpy(playerMovePtr, userResponse);
				free(userResponse);
				return;
			}
		}
		free(userResponse);
	}
}

/*
 * asks the player what piece to promote the pawn to.
 * returns the piecetype once a valid type is entered.
 */
char promptOnPawnPromote() {
	char *userInput;
	while(TRUE) {
		userInput = (char *)prompt("Pawn Promotion! Enter a new piece type:", 
                                   STRING);

		if(strlen(userInput) == 1) {
			char pieceName = tolower(userInput[0]); 
			switch(pieceName) {
				case 'r': case 'n':
				case 'b': case 'q':
					free(userInput);
					return pieceName;
			}
		}
		free(userInput);
	}
}
