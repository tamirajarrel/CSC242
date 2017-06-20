#include "Board.h"

const int BOARD_SIZE = 3;
const int WINNER_SIZE = BOARD_SIZE*2+2;
//typedef uint64_t board;
void printBoard(int[BOARD_SIZE][BOARD_SIZE]);
void numToXY(int, int&, int&);
void promptMove(int & ,int &, int&, Board[BOARD_SIZE*BOARD_SIZE]);
int updateBoard(Board &, int , int , bool);
void printWinners(int [WINNER_SIZE]);
int pickNext(Board[BOARD_SIZE*BOARD_SIZE] , int x, int y, bool isPlayer, int maxDepth, int pieces, int &retx, int &rety	, bool isX, int alpha, int beta, int boardPick, int & retBoard, int heuristicSum, int prevBoard);
int updateWinner(Board &);
int XYToNum(int, int);
void gameLoop(Board[BOARD_SIZE*BOARD_SIZE], bool maximize, int maxDepth, int pieces, bool isX, int heuristicSum, int nextPlay);
bool checkWinner(Board &, int);
void undoBoard(Board &, int x, int y, bool isX);
void printBigBoard(Board[BOARD_SIZE*BOARD_SIZE]);
