#include <stdio.h>
#include <cstdint>

/*State: 2D array of board
initiative

Actions: avaiable spots to place x's and o's

Transition Model: update board state

Initi2
al State: Empty Board, x goes first

Goal State: End when 3 in a row, Horiz, Vert, Diag
			No Empty Squares + no Goal = draw

*/


#include "TTT.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include <time.h>
#include <algorithm>
using namespace std;

int main(){

	cerr << "Pick a side: ('X' or 'O' )" << endl;
	char side;
	cin >> side;
	while(side != 'X' && side != 'O' && side != 'x' && side!= 'o'){
		cerr << "Please, pick a side: ('X' or 'O') " << endl;
		cin >> side;
	}

	cerr << "You have chosen " << ((side == 'x' || side == 'X') ? "X" : "O") << " as your side." << endl;

	//Array of Board, Initialized to Blank
	//-1 = Empty
	// 0 = "0"
	// 1 = "X"             top     middle      bottom
	int board[3][3] = {{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}};

	//Array of Places on Board Still Open
	//bool actions[3][3] = {{true,true,true},{true,true,true}, {true,true,true}};

	//Array holding the winners on each row/diagonal
	//0-2: horiz
	//3-5: vert
	//6-7: diag
	int winner[BOARD_SIZE*2 + 2] = {0};


	int posx, posy;
	bool maximize = false;

	if(side == 'x' || side == 'X'){
		gameLoop(board, winner, false, 10, 0, false);
	}else{
		int retx, rety;//    placeholders
		//clock_t t = clock();
		//for(int i = 0; i < 100; i++)
		pickNext(board, winner, 0, 0, true, 10 , 0, retx, rety, true, -100, 100 );
		//t = clock() - t;
		//cerr << "\n" << ((float)t)/CLOCKS_PER_SEC << endl;
		cout << XYToNum(retx, rety);
		updateBoard(board, winner, retx, rety, true);
		gameLoop(board, winner, true, 10, 1, true);
	}


	//cout << posx << " " << posy << endl;
	//actions[posx][posy] = false;



}


int counter = 0;

void promptMove( int &x, int & y, int board[BOARD_SIZE][BOARD_SIZE]){
	cerr << "Please make a move: (1-9)" << endl;
	int move;
	cin >> move;

	int retx, rety;
	numToXY(move, retx, rety);

	while((!(abs(move-1) < 9)) || board[retx][rety] != -1 ){


		cerr << "Invalid: Please make a move: (1-9)" << endl;
		cin >> move;
		numToXY(move, retx, rety);
	}
	numToXY(move, x, y);
}


void gameLoop(int board[BOARD_SIZE][BOARD_SIZE], int winner[WINNER_SIZE], bool maximize, int maxDepth, int pieces, bool isX){
	int posx, posy;
	promptMove(posx, posy, board);
	updateBoard(board, winner, posx, posy, !isX);
	++pieces;
	if(checkWinner(winner, pieces)){
		return;
	}



	int retx, rety;
	pickNext(board, winner, posx, posy, maximize, maxDepth , pieces, retx, rety, isX, -100, 100);
	cout << XYToNum(retx, rety);
	if(DEBUG){printBoard(board);}
	updateBoard(board, winner, retx, rety, isX);
	++pieces;
	if(checkWinner(winner, pieces)){
		return;
	}

	gameLoop(board, winner, maximize, maxDepth, pieces, isX);
}






bool checkWinner(int winner[WINNER_SIZE], int pieces){
	if(updateWinner(winner) == 1){
			cerr << "I Win";
			return true;
		}else if(updateWinner(winner) == -1){
			cerr << "I Lose";
			return true;
		}else if(pieces >= 9){
			cerr << "it's a draw";
			return true;
		}else{
			return false;
		}

}


int pickNext(int board[BOARD_SIZE][BOARD_SIZE], int winner[WINNER_SIZE], int x, int y, bool maximize, int maxDepth, int pieces, int &retx, int &rety, bool isX, int alpha, int beta	){
	if(DEBUG)
	cout << ++counter << " maximize: " << maximize << endl;
	int winnerValue = updateWinner(winner);

	//char c;
	//cin >> c;

	//If weve found a winner or are at max recursion depth, exit the program
	if(winnerValue != 0 || maxDepth == 0 || pieces >= 9)
		return winnerValue;//+heuristic

	//The Minimax part
	int m = (maximize) ? 1 : -1;

	int scoreMax = -2;//lower than any possible move

	for(int i = 0; i < BOARD_SIZE; i++){
		for(int j = 0; j < BOARD_SIZE; j++){

			if(board[i][j] == -1){//will never not occur because of pieces variable

				updateBoard(board, winner, i, j, isX);
				int retx2, rety2;
				int newMax = m * pickNext( board, winner,i,j,!maximize , maxDepth - 1, pieces+1, retx2, rety2, !isX, alpha, beta);

				undoBoard(board, winner, i, j , isX);

				if(newMax > scoreMax){
					scoreMax = newMax;
					retx = i;
					rety = j;
				}

				if(maximize){
					alpha = max(alpha, newMax);
					if(beta <= alpha){
						goto ABSKIP; // fight me
					}
				}else{
					beta = min(-1*newMax, beta);
					if(beta <= alpha){
						goto ABSKIP;
					}

				}



			}


		}
	}



	if(DEBUG){
		cout << m*scoreMax << ": " << pieces << " " << retx << " " << rety << " " << alpha << " " << beta << endl;
		printBoard(board);
	}

	ABSKIP:
	return m*scoreMax;
}









//
//Update Board and check the terminal condition of a board
//must be done together so that we can optimize out terminal checks to just those affected
//1 = Player win
//0 = draw
//-1 = Enemy win
//null: nonterminal
int updateBoard(int board[BOARD_SIZE][BOARD_SIZE], int winner[WINNER_SIZE], int x, int y, bool isX){
	board[x][y] = (isX) ? 1 : 0;

	if(DEBUG)printBoard(board);

	int d = (isX) ? 1 : -1;
	winner[3+x]+= d;
	winner[y]+= d;
	if(y==x){
		winner[BOARD_SIZE*2]+= d;
	}
	if(y==0 && x==2 || y == 2 && x == 0 || ( y == 1 && x == 1)){
		winner[BOARD_SIZE*2+1]+= d;
	}
	if(DEBUG)printWinners(winner);

}


void undoBoard(int board[BOARD_SIZE][BOARD_SIZE], int winner[WINNER_SIZE], int x, int y, bool isX){
	board[x][y] = -1;


	int d = (isX) ? -1 : 1;
	winner[3+x]+= d;
	winner[y]+= d;
	if(y==x){
		winner[BOARD_SIZE*2]+= d;
	}
	if(y==0 && x==2 || y == 2 && x == 0 || ( y == 1 && x == 1)){
		winner[BOARD_SIZE*2+1]+= d;
	}

}


//update winner array based on move
//if isPlayer, add, else subtract
int updateWinner(int winner[WINNER_SIZE]){
	for(int i = 0; i < WINNER_SIZE; i++){
		if(winner[i] >= 3){//X's
			return 1;
		}
		if(winner[i] <= -3){//O's
			return -1;
		}
	}
	return 0;
}






//Syntax is correct for passing array as pointer
//Method prints board as X's and O's
//Is maybe inverted dependent on how you input the board
void printBoard(int board[BOARD_SIZE][BOARD_SIZE]){
	for(int i = 0; i < BOARD_SIZE; i++){
		for(int j = 0; j < BOARD_SIZE; j++){
			if(board[i][j] == -1 )
				printf("_");
			else if(board[i][j] == 0)
				printf("0");
			else
				printf("1");
			printf(" ");
		}
		printf("\n");
	}
}

//utility function to turn number into board coordinates
void numToXY(int num, int &retx, int &rety){
	switch (num){
		case 1:
			retx = 0;
			rety = 0;
			break;
		case 2:
			retx = 0;
			rety = 1;
			break;
		case 3:
			retx = 0;
			rety = 2;
			break;
		case 4:
			retx = 1;
			rety = 0;
			break;
		case 5:
			retx = 1;
			rety = 1;
			break;
		case 6:
			retx = 1;
			rety = 2;
			break;
		case 7:
			retx = 2;
			rety = 0;
			break;
		case 8:
			retx = 2;
			rety = 1;
			break;
		case 9:
			retx = 2;
			rety = 2;
			break;
		default:
			retx = -1;
			rety = -1;

	}
}

int XYToNum(int x, int y){
	if(x == 0 && y == 0){
		return 1;
	}
	if(x == 1 && y == 0){
		return 4;
	}
	if(x == 2 && y == 0){
		return 7;
	}
	if(x == 0 && y == 1){
		return 2;
	}
	if(x == 1 && y == 1){
		return 5;
	}
	if(x == 2 && y == 1){
		return 8;
	}
	if(x == 0 && y == 2){
		return 3;
	}
	if(x == 1 && y == 2){
		return 6;
	}
	if(x == 2 && y == 2){
		return 9;
	}
	return -1;//something went wrong
}

void printWinners(int w[WINNER_SIZE]){
	cout << "\n";
	for(int i = 0; i < WINNER_SIZE; i++){
		cout << " " << w[i] << " " ;
	}
	cout << "\n";
}
