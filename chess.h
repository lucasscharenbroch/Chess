#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define WHITE 1
#define BLACK 0

/* for prompts.c */
#define BOOL 0
#define INT 1
#define STRING 2

#define STALEMATE 0
#define WHITE_CHECKMATE 1
#define BLACK_CHECKMATE 2
#define CONTINUE 3

#define KING_SIDE_CASTLE "KCSL"
#define QUEEN_SIDE_CASTLE "QCSL"

/* booleans */
#define TRUE 1
#define FALSE 0


/*
 * stores information about the game:
 *       the board,
 *       turn, user preference, captured pieces, and scores
 */
typedef struct _gameState {
    char board[8][8];
    int highlighted[8][8]; /* array of booleans that represents which tiles should be hihlighted */

    int turn; /* WHITE or BLACK */
    int printInvertedBoard; /* flip the board during black's turn ? */

    char whiteCapturedPieces[17];
    char blackCapturedPieces[17];

    int whiteScore; /* based on captured pieces and their respective "Scores" */
    int blackScore; /* based on captured pieces and their respective "Scores" */

} GameState;

/* game.c */
void playGame(GameState *gamePtr);
int pieceIsWhite(char piece);
char tempExecuteMove(GameState *gamePtr, char *move, int color); 
void reverseMove(GameState *gamePtr, char *move, int color, char overwrittenPiece);
int letterToCol(char letter);
int colToLetter(int col);
int letterToRow(char letter);
int rowToLetter(int row);
void freeStringArray(char **arr, int length);

/* printing.c */
void printGameInfo(GameState *gamePtr);
void promptForMove(GameState *gamePtr, char *playerMovePtr);
void consoleSetup();
void setHighlights(GameState *gamePtr, char *coord);
void clearHighlights(GameState *gamePtr);
char promptOnPawnPromote();

/* prompts.c */
void *prompt(char *promptString, int type);

/* moves.c */
int getPieceLegalMoves(GameState *gamePtr, char **moveList, int row, int col);
int getAllLegalMoves(GameState *gamePtr, char **moveArr, int color);
int isKingInCheck(GameState *gamePtr, int color);
int canCastle(GameState *gamePtr, int color, int isKingSide);
int putsKingInCheck(GameState *gamePtr, char *move, int color);
