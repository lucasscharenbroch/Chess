#include "chess.h"


/*
 * free string array:
 * frees all char * elements of an array,
 * then frees the array itself.
 */
void freeStringArray(char **arr, int length) {
	for(int i = 0; i < length; i++) {
		free(arr[i]);
	}
	free(arr);
}

/*
 * letter to col:
 * converts a letter to the col number
 */
int letterToCol(char letter) {
	return letter - 'a';
}

/*
 * col to letter:
 * converts the col number to a letter
 */
int colToLetter(int col) {
	return col + 'a';
}

/*
 * letter to row:
 * converts a letter to the row number
 */
int letterToRow(char letter) {
    return 8 - letter + '0';
}

/*
 * row to letter:
 * converts the row number to a letter
 */
int rowToLetter(int row) {
    return -row + '0' + 8;
}

/*
 * pieceIsWhite:
 * is the piece white (uppercase) ?
 */
int pieceIsWhite(char piece) {
	return(tolower(piece) != piece);
}

/*
 * returns the "value score" of the chess piece
 */
int pieceValue(char pieceName) {
	switch(tolower(pieceName)) {
		case 'p':
			return 1;
		case 'b': case 'n':
			return 3;
		case 'r':
			return 5;
		case 'q':
			return 9;
	}
}

/*
 * checks the board for checkmate and stalemate:
 * returns CONTINUE (neither), CHECKMATE, or STALEMATE
 */
int checkLosingCondition(GameState *gamePtr) {
	char **possibleOpponentMoves = malloc(150 * sizeof(char *));	
	int numOpponentMoves = getAllLegalMoves(gamePtr, possibleOpponentMoves, 
                                            !gamePtr->turn);

	if(numOpponentMoves == 0) {
		if(isKingInCheck(gamePtr, !gamePtr->turn)) {
			return gamePtr->turn ? WHITE_CHECKMATE : BLACK_CHECKMATE;
		} else {
			return STALEMATE;
		}
	}

	/* free possibleOpponentMoves */
	freeStringArray(possibleOpponentMoves, numOpponentMoves);

	return CONTINUE;
}

/*
 * moves the piece on the board,
 * and sets player scores/captured accordingly.
 * returns: CONTINUE, STALEMATE, or CHECKMATE
 */
int executeMove(GameState *gamePtr, char *move) {
	/* make castles */
	if(strcmp(move, KING_SIDE_CASTLE) == 0) {
		int row = gamePtr->turn == WHITE ? 7 : 0;

		gamePtr->board[row][5] = gamePtr->board[row][7];
		gamePtr->board[row][6] = gamePtr->board[row][4];
		gamePtr->board[row][4] = ' ';
		gamePtr->board[row][7] = ' ';

		goto ret;
	} else if(strcmp(move, QUEEN_SIDE_CASTLE) == 0) { 
		int row = gamePtr->turn == WHITE ? 7 : 0;

		gamePtr->board[row][3] = gamePtr->board[row][0];
		gamePtr->board[row][2] = gamePtr->board[row][4];
		gamePtr->board[row][4] = ' ';
		gamePtr->board[row][0] = ' ';

		goto ret;
	}

	/* regular two-coordinate move */
	int sourceCol = letterToCol(move[0]);
	int sourceRow = letterToRow(move[1]);	
	int destCol = letterToCol(move[2]);
	int destRow = letterToRow(move[3]);

	/* update scores and captured */
	char movingPiece = gamePtr->board[sourceRow][sourceCol];
	char capturedPiece = gamePtr->board[destRow][destCol]; 
	if(capturedPiece != ' ') {
		if(gamePtr->turn == WHITE) {
			int numCaptured = strlen(gamePtr->whiteCapturedPieces);
			gamePtr->whiteCapturedPieces[numCaptured] = capturedPiece;
			gamePtr->whiteCapturedPieces[numCaptured+ 1 ] = '\0';

			gamePtr->whiteScore += pieceValue(capturedPiece); 
		}else {
			int numCaptured = strlen(gamePtr->blackCapturedPieces);
			gamePtr->blackCapturedPieces[numCaptured] = capturedPiece;
			gamePtr->blackCapturedPieces[numCaptured + 1] = '\0';

			gamePtr->blackScore += pieceValue(capturedPiece);
		}
	}

	/* check for pawn promotion */
	int isEndRow = destRow == 7 || destRow == 0;
	int isPawn = tolower(movingPiece) == 'p';
	if(isEndRow && isPawn) {
		movingPiece = promptOnPawnPromote();

		if(gamePtr->turn == WHITE) /* set the piece to the correct color */
			movingPiece -= 'a' - 'A';
	}


	/* make move */
	gamePtr->board[destRow][destCol] = movingPiece; 
	gamePtr->board[sourceRow][sourceCol] = ' ';

	/* return losing condition */
	ret:
	return checkLosingCondition(gamePtr);
}

/*
 * the move is executed, only changing the board
 * (no scores/captures are updated)
 * returns the overwritten piece (to be passed into reverseMove)
 */
char tempExecuteMove(GameState *gamePtr, char *move, int color) {
	/* castles */
	if(strcmp(move, KING_SIDE_CASTLE) == 0) {
		int row = color == WHITE ? 7 : 0;

		gamePtr->board[row][5] = gamePtr->board[row][7];
		gamePtr->board[row][6] = gamePtr->board[row][4];
		gamePtr->board[row][4] = ' ';
		gamePtr->board[row][7] = ' ';
		
		return ' ';
	} else if(strcmp(move, QUEEN_SIDE_CASTLE) == 0) { 
		int row = color == WHITE ? 7 : 0;

		gamePtr->board[row][3] = gamePtr->board[row][0];
		gamePtr->board[row][2] = gamePtr->board[row][4];
		gamePtr->board[row][4] = ' ';
		gamePtr->board[row][0] = ' ';

		return ' ';
	}

	/* regular two-coordinate move */
	int sourceCol = letterToCol(move[0]);
	int sourceRow = letterToRow(move[1]);	
	int destCol = letterToCol(move[2]);
	int destRow = letterToRow(move[3]);

	char overwrittenPiece = gamePtr->board[destRow][destCol]; 

	/* make move */
	gamePtr->board[destRow][destCol] = gamePtr->board[sourceRow][sourceCol]; 

	return overwrittenPiece;
}

/*
 * reverses a move that was temporarily executed.
 */
void reverseMove(GameState *gamePtr, char *move, int color, char overwrittenPiece) {
	/* castles */
	if(strcmp(move, KING_SIDE_CASTLE) == 0) {
		int row = color == WHITE ? 7 : 0;

		gamePtr->board[row][4] = gamePtr->board[row][6];
		gamePtr->board[row][7] = gamePtr->board[row][5];
		gamePtr->board[row][5] = ' ';
		gamePtr->board[row][6] = ' ';
		
		return;
	} else if(strcmp(move, QUEEN_SIDE_CASTLE) == 0) { 
		int row = color == WHITE ? 7 : 0;

		gamePtr->board[row][4] = gamePtr->board[row][2];
		gamePtr->board[row][0] = gamePtr->board[row][3];
		gamePtr->board[row][3] = ' ';
		gamePtr->board[row][2] = ' ';

		return;
	}

	/* regular two-coordinate move */
	int sourceCol = letterToCol(move[0]);
	int sourceRow = letterToRow(move[1]);	
	int destCol = letterToCol(move[2]);
	int destRow = letterToRow(move[3]);

	/* reverse move */
	gamePtr->board[sourceRow][sourceCol] = gamePtr->board[destRow][destCol];
	gamePtr->board[destRow][destCol] = overwrittenPiece; 

}


/*
 * checks if move is legal (based on the board)
 */
int moveIsLegal(GameState *gamePtr, char *move) {
	if(strcmp(move, KING_SIDE_CASTLE) == 0) {
		return canCastle(gamePtr, gamePtr->turn, TRUE);
	} else if(strcmp(move, QUEEN_SIDE_CASTLE) == 0) {
		return canCastle(gamePtr, gamePtr->turn, FALSE); 
	}

	int sourceCol = letterToCol(move[0]);
	int sourceRow = letterToRow(move[1]);	
	int destCol = letterToCol(move[2]);
	int destRow = letterToRow(move[3]);

	/* make sure piece matches player color */
	char pieceType = gamePtr->board[sourceRow][sourceCol];
	if(pieceType == ' ' || (pieceIsWhite(pieceType) != gamePtr->turn )) {
		return FALSE;
	}

	/* get pieceLegalMoves */
	char **possiblePieceMoves = malloc(30 * sizeof(char *));
	int numPieceMoves = getPieceLegalMoves(gamePtr, possiblePieceMoves, 
                                           sourceRow, sourceCol); 

	for(int i = 0; i < numPieceMoves; i++) {
		if(possiblePieceMoves[i][2] == colToLetter(destCol) && 
           possiblePieceMoves[i][3] == rowToLetter(destRow)) {
			freeStringArray(possiblePieceMoves, numPieceMoves);
			return TRUE;
		}
	}

	freeStringArray(possiblePieceMoves, numPieceMoves);
	return FALSE;
}

/*
 * runs the basic game loop of gamePtr:
 * 	calls printGameInfo
 * 	prompts the user for a move (a move or request for possible mvoes)
 * 	executes that move
 * 	checks for game ending conditions
 * 	swaps the turn
 * prints the losing condition
 *
 * returns:
 * int: losing condition of the game (STALEMATE or 
 *                                    WHITE_CHECKMATE or BLACK_CHECKMATE)
 */
void playGame(GameState *gamePtr) {
	int losingCondition;
	while(TRUE) {
		printGameInfo(gamePtr);	

		char playerMove[] = "    ";
		promptForMove(gamePtr, playerMove);

		if(!moveIsLegal(gamePtr, playerMove)) {
			continue; /* skip over the (execute and turn switch) 
                         and prompt again */	
		}

		if((losingCondition = executeMove(gamePtr, playerMove)) != CONTINUE) {
			break;
		}
		
		gamePtr->turn = !gamePtr->turn; /* flip turn */

	}
	printGameInfo(gamePtr);	
	switch(losingCondition) {
		case STALEMATE:
			printf("Stalemate!\n");
			break;
		case WHITE_CHECKMATE:
			printf("White checkmate!\n");
			break;
		case BLACK_CHECKMATE:
			printf("Black checkmate!\n");
			break;
	}

}
