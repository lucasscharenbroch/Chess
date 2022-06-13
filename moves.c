#include "chess.h"

#define COLLISION 0
#define NO_COLLISION 1

/*
 * contains the current
 * and destination coordinates
 * of a moving piece
 */
typedef struct _moveConjecture {
	/* current location */
	int row;
	int col;

	/* destination */
	int dRow;
	int dCol;

	char pieceName;
} MoveConjecture;


/*
 * allocates space for a copy of moveStr,
 * and returns a pointer to that copy
 */
char *newMove(const char *moveToCopy) {
	char *movePtr = malloc(strlen(moveToCopy) + 1);
	strcpy(movePtr, moveToCopy);

	return movePtr;
}


/*
 * translates a MoveConjecture to a string
 * that represents the move.
 */
char *getMoveStr(MoveConjecture move) {
	static char moveStr[] = "    ";

	/* moveStr[0:1] = source coord */
	moveStr[0] = colToLetter(move.col);
	moveStr[1] = rowToLetter(move.row); 

	/* moveStr[2:3] = destination coord */
	moveStr[2] = colToLetter(move.dCol);
	moveStr[3] = rowToLetter(move.dRow); 

	return moveStr;
}

/* 
 * global: moveType- holds the move's collision status (
 * NO_COLLISION or COLLISION
 */ 
int moveType;

/**
 * noCollision:
 * checks if destinationPiece is blank,
 * sets moveType accordingly
 *
 * recieves:
 * destinationPiece, movingPiece
 *
 * returns
 * collision?
 */
int noCollision(char destinationPiece, char movingPiece) {
	return (moveType = destinationPiece == ' ');	
}

/*
 * returns TRUE if destinationPiece is has
 * a different color (case) than movingPiece.
 * sets moveType accordingly
 */
int onlyUnfriendlyCollision(char destinationPiece, char movingPiece) {
	if(moveType = destinationPiece == ' ') {
		return FALSE;
	}

	int isWhite = pieceIsWhite(movingPiece);	
	int destIsWhite = pieceIsWhite(destinationPiece);	

	return (isWhite != destIsWhite);
}

/*
 * returns TRUE if destinationPiece is ' ' or has
 * a different color (case) than movingPiece.
 * sets moveType accordingly
 */
int noFriendlyCollision(char destinationPiece, char movingPiece) {
	if(moveType = destinationPiece == ' ') {
		return TRUE;
	}

	int isWhite = pieceIsWhite(movingPiece);	
	int destIsWhite = pieceIsWhite(destinationPiece);	

	if(isWhite != destIsWhite) { /* unfriendly collision */
		return TRUE;
	}
	
	/* friendly collision */
	return FALSE;
}

/*
 * adds move to moveArr (incrementing numMovesPtr) if condition is met.
 * (according to the sate of gamePtr)
 */
void conditionalAddMove(GameState *gamePtr, char **moveArr, int *numMovesPtr, 
                        MoveConjecture move, int (* condition)(char, char)) {
	if(condition(gamePtr->board[move.dRow][move.dCol], move.pieceName)) {
		moveArr[(*numMovesPtr)++] = newMove(getMoveStr(move)); /* add move */
	}
}


/*
 * finds moves that can be made by the pawn
 * whose position is refered to by move.
 * adds those moves to moveArr, and adjusts numMovesPtr accordingly.
 */
void pawnMoves(GameState *gamePtr, char **moveArr, int *numMovesPtr, 
               MoveConjecture move) {

	int isWhite = pieceIsWhite(move.pieceName);
	int canDoubleJump = (!isWhite && move.row == 1) || 
                        (isWhite && move.row == 6);
	int regularJump = (isWhite) ? -1 : 1;

	move.dCol = move.col;

	/* double jump */
	move.dRow = move.row + 2 * regularJump;
	if(canDoubleJump) {
		conditionalAddMove(gamePtr, moveArr, numMovesPtr, move, noCollision);
	}

	/* single jump */
	move.dRow = move.row + 1 * regularJump;
	conditionalAddMove(gamePtr, moveArr, numMovesPtr, move, noCollision);


	/* diagonal captures */
	move.dCol = move.col + 1;
	if(move.dCol < 8 && move.dCol >= 0)
		conditionalAddMove(gamePtr, moveArr, numMovesPtr, move, 
                           onlyUnfriendlyCollision);

	move.dCol = move.col - 1;
	if(move.dCol < 8 && move.dCol >= 0)
		conditionalAddMove(gamePtr, moveArr, numMovesPtr, move, 
                           onlyUnfriendlyCollision);
}

/*
 * repeatedly advances the piece (whose coordinate
 * is in move) in the direction (based on rOffest
 * and cOffset)- moves are added to moveArr until an invalid
 * move is reached. 
 */
void repeatedDirectionalMove(GameState *gamePtr, char **moveArr, 
                             int *numMovesPtr, MoveConjecture move, 
                             int rOffset, int cOffset) {

	int r = move.row + rOffset;
	int c = move.col + cOffset;
	while(r >= 0 && r < 8 && c >= 0 && c < 8) {
		move.dRow = r;
		move.dCol = c;
		conditionalAddMove(gamePtr, moveArr, numMovesPtr, move, 
                           noFriendlyCollision);
		if(moveType != NO_COLLISION)
			return;

		r += rOffset;
		c += cOffset;
	}
	
}

/*
 * finds moves that can be made by the rook
 * whose position is refered to by move.
 * adds those moves to moveArr, and adjusts numMovesPtr accordingly.
 */
void rookMoves(GameState *gamePtr, char **moveArr, int *numMovesPtr, 
               MoveConjecture move) {
    /* left */
	repeatedDirectionalMove(gamePtr, moveArr, numMovesPtr, move, 0, -1);
    /* right */
	repeatedDirectionalMove(gamePtr, moveArr, numMovesPtr, move, 0, 1);
    /* up */
	repeatedDirectionalMove(gamePtr, moveArr, numMovesPtr, move, -1, 0);
    /* down */
	repeatedDirectionalMove(gamePtr, moveArr, numMovesPtr, move, 1, 0);
}

/*
 * finds moves that can be made by the knight
 * whose position is refered to by move.
 * adds those moves to moveArr, and adjusts numMovesPtr accordingly.
 */
void knightMoves(GameState *gamePtr, char **moveArr, int *numMovesPtr, 
                 MoveConjecture move) {
	int knightMoveOffsets[8][2] = {{1, 2}, {1, -2}, {2, 1}, {2, -1}, {-1, 2}, 
                                   {-1, -2}, {-2, 1}, {-2, -1}};

	for(int i = 0; i < 8; i++) {
		move.dRow = move.row + knightMoveOffsets[i][0];
		move.dCol = move.col + knightMoveOffsets[i][1];
		if(move.dRow >= 0 && move.dRow < 8 && move.dCol >= 0 && 
           move.dCol < 8){ /* move fits in board */
			conditionalAddMove(gamePtr, moveArr, numMovesPtr, 
                               move, noFriendlyCollision);
        }
	}
}


/*
 * finds moves that can be made by the bishop
 * whose position is refered to by move.
 * adds those moves to moveArr, and adjusts numMovesPtr accordingly.
 */
void bishopMoves(GameState *gamePtr, char **moveArr, int *numMovesPtr, 
                 MoveConjecture move) {
    /* northwest */
	repeatedDirectionalMove(gamePtr, moveArr, numMovesPtr, move, -1, -1);
    /* northeast */
	repeatedDirectionalMove(gamePtr, moveArr, numMovesPtr, move, -1, 1);
    /* southwest */
	repeatedDirectionalMove(gamePtr, moveArr, numMovesPtr, move, 1, -1); 
    /* southeast */
	repeatedDirectionalMove(gamePtr, moveArr, numMovesPtr, move, 1, 1); 
}

/*
 * finds moves that can be made by the queen
 * whose position is refered to by move.
 * adds those moves to moveArr, and adjusts numMovesPtr accordingly.
 */
void queenMoves(GameState *gamePtr, char **moveArr, int *numMovesPtr, 
                MoveConjecture move) {
	rookMoves(gamePtr, moveArr, numMovesPtr, move);
	bishopMoves(gamePtr, moveArr, numMovesPtr, move);
}

/*
 * finds moves that can be made by the king
 * whose position is refered to by move.
 * adds those moves to moveArr, and adjusts numMovesPtr accordingly.
 */
void kingMoves(GameState *gamePtr, char **moveArr, int *numMovesPtr, 
               MoveConjecture move) {
	int kingMoveOffsets[8][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, 
                                 {1 , 1}, {1, -1}, {-1, 1}, {-1, -1}};

	for(int i = 0; i < 8; i++) {
		move.dRow = move.row + kingMoveOffsets[i][0];
		move.dCol = move.col + kingMoveOffsets[i][1];
		if(move.dRow >= 0 && move.dRow < 8 && move.dCol >= 0 &&
           move.dCol < 8) { /* move fits in board */
			conditionalAddMove(gamePtr, moveArr, numMovesPtr, 
                               move, noFriendlyCollision);
        }
	}
}

/*
 * gets all moves allowed (disregaurding checks) for a given piece,
 * according to the state of the board.
 */
void getPieceMoves(GameState *gamePtr, char **moveArr, int *numMovesPtr, 
                   int row, int col) {
	/* set up possible move struct */
	MoveConjecture possibleMove;
	possibleMove.row = row;
	possibleMove.col = col;
	possibleMove.pieceName = gamePtr->board[row][col];


	/* add all possible moves based on pieceType */
	switch(tolower(possibleMove.pieceName)) {
		case 'p':
			return pawnMoves(gamePtr, moveArr, numMovesPtr, possibleMove);
			break;
		case 'r':
			return rookMoves(gamePtr, moveArr, numMovesPtr, possibleMove);
			break;
		case 'n':
			return knightMoves(gamePtr, moveArr, numMovesPtr, possibleMove);
			break;
		case 'b':
			return bishopMoves(gamePtr, moveArr, numMovesPtr, possibleMove);
			break;
		case 'q':
			return queenMoves(gamePtr, moveArr, numMovesPtr, possibleMove);
			break;
		case 'k':
			return kingMoves(gamePtr, moveArr, numMovesPtr, possibleMove);
			break;

	}	
}

/*
 * is the castle of respective type and color a piece-legal move?
 */
int canCastle(GameState *gamePtr, int color, int isKingSide) {
	int piecesAlign;

	int row = (color == WHITE) ? 7 : 0;
	char king = (color == WHITE) ? 'K' : 'k';
	char rook = (color == WHITE) ? 'R' : 'r'; 

	if(isKingSide) {
		piecesAlign = gamePtr->board[row][4] == king && 
            gamePtr->board[row][7] == rook &&
			gamePtr->board[row][5] == ' ' && gamePtr->board[row][6] == ' ';

	} else {
		piecesAlign = gamePtr->board[row][4] == king && 
            gamePtr->board[row][0] == rook &&
			gamePtr->board[row][5] == ' ' && gamePtr->board[row][6] == ' ';
	}
	return piecesAlign;
}


/*
 * checks for castle elegibility, and adds
 * the castles to moveArr accordingly.
 */
void getCastles(GameState *gamePtr, char **moveArr, int *numMovesPtr, 
                int color) {

	if(canCastle(gamePtr, color, TRUE)) {
		moveArr[(*numMovesPtr)++] = newMove(KING_SIDE_CASTLE);
	}

	if(canCastle(gamePtr, color, FALSE)) {
		moveArr[(*numMovesPtr)++] = newMove(QUEEN_SIDE_CASTLE);
	}
}

/*
 * gets all moves that are allowed (disregaurding checks) by a given color,
 * according to the state of the board
 */
int getAllMoves(GameState *gamePtr, char **moveArr, int color) {
	int numMoves = 0;
	char piece;
	for(int row = 0; row < 8; row++) {
		for(int col = 0; col < 8; col++) {
			piece = gamePtr->board[row][col];

			if(piece != ' ' && pieceIsWhite(piece) == color) {
				getPieceMoves(gamePtr, moveArr, &numMoves, row, col);
			}
		}
	}

	getCastles(gamePtr, moveArr, &numMoves, color);
	return numMoves;
}

/*
 * is the king in check?
 */
int isKingInCheck(GameState *gamePtr, int color) {
	char **opposingMoves = malloc(150 * sizeof(char *));
	int numOpposingMoves = getAllMoves(gamePtr, opposingMoves, !color);
	char playersKing = (color == WHITE) ? 'K' : 'k';

	int rDest, cDest;
	for(int i = 0; i < numOpposingMoves; i++) {
		cDest = letterToCol(opposingMoves[i][2]);
		rDest = letterToRow(opposingMoves[i][3]);

		if(gamePtr->board[rDest][cDest] == playersKing) {
			freeStringArray(opposingMoves, numOpposingMoves);
			return TRUE;
		}	
	}

	freeStringArray(opposingMoves, numOpposingMoves);
	return FALSE;
}


/*
 * recieves a move, checks if that move puts the king in check.
 */
int putsKingInCheck(GameState *gamePtr, char *move, int color) {
	char overwrittenPiece;
	overwrittenPiece = tempExecuteMove(gamePtr, move, color);  

	int kingGotChecked = isKingInCheck(gamePtr, color);
	
	reverseMove(gamePtr, move, color, overwrittenPiece);

	return kingGotChecked;
}

/*
 * gets all the moves that are legal for a given
 * color
 */
int getAllLegalMoves(GameState *gamePtr, char **moveArr, int color) {
	char **possibleMoveArr = malloc(150 * sizeof(char *));
	int numPossibleMoves = getAllMoves(gamePtr, possibleMoveArr, color);

    int numLegalMoves = 0;
	for(int i = 0; i < numPossibleMoves; i++) {
		if(!putsKingInCheck(gamePtr, possibleMoveArr[i], color)) {
			moveArr[numLegalMoves++] = possibleMoveArr[i];
            possibleMoveArr[i] = NULL;
		}
	}


	freeStringArray(possibleMoveArr, numPossibleMoves);

	return numLegalMoves;
}

/*
 * gets all moves that are legal for a given piece
 */
int getPieceLegalMoves(GameState *gamePtr, char **moveArr, int row, int col) {
	int numMoves = 0;
	int numLegalMoves = 0;
	getPieceMoves(gamePtr, moveArr, &numMoves, row, col);
	int color = pieceIsWhite(gamePtr->board[row][col]);

	for(int i = 0; i < numMoves; i++) {
		if(!putsKingInCheck(gamePtr, moveArr[i], color)) {
			moveArr[numLegalMoves++] = moveArr[i];
		}	
	}

	return numLegalMoves;
}
