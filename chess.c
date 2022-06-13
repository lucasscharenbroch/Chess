#include "chess.h"


/*
 * copies one board to another
 */
void boardCopy(char boardDest[8][8], char boardSource[8][8]) {
	for(int row = 0; row < 8; row++) {
		for(int col = 0; col < 8; col++) {
			boardDest[row][col] = boardSource[row][col];
		}
	}
}

/*
 * sets the fields of a new GameState based on default values
 * and user input.
 */
GameState *initNewGame() {
	GameState *gamePtr = malloc(sizeof(GameState));

	/* init game's members */
	gamePtr->printInvertedBoard = *(int *) prompt("\nFlip the board during black's turn? (y/n)", BOOL);
	gamePtr->turn = WHITE; /* white starts the game */
	gamePtr->whiteCapturedPieces[0] = '\0';
	gamePtr->blackCapturedPieces[0] = '\0';
	gamePtr->whiteScore = 0;
	gamePtr->blackScore = 0;
	clearHighlights(gamePtr);

	/* board */
	char startingBoard[8][8] = {
		'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r',
		'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
		' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
		'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'
	};
	boardCopy(gamePtr->board, startingBoard);

	return gamePtr;
}

/*
 * prints welcome messages, initializes a new GameState, runs playGame()
 * prints exit message.
 */
int main() {
	consoleSetup();
	printf("Welcome to chess!\n");

	GameState *gamePtr = initNewGame();

	playGame(gamePtr);

	free(gamePtr);

	printf("Thanks for playing!\n");
	return 0;
}
