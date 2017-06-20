#include <stdio.h>
#include <bitset>
/*State: 2D array of board
initiative

Actions: avaiable spots to place x's and o's

Transition Model: update board state

Initi2
al State: Empty Board, x goes first

Goal State: End when 3 in a row, Horiz, Vert, Diag
			No Empty Squares + no Goal = draw

*/

const int BOARD_SIZE = 3;
const int WINNER_SIZE = BOARD_SIZE*2+2;
const int DEBUG = false;
const int TIMING = true;
//typedef uint64_t board;
void printBoard(int[BOARD_SIZE][BOARD_SIZE]);
void numToXY(int, int&, int&);
uint64_t promptMove(int &, int&);
void updateBoard(uint64_t &boardX, uint64_t &boardY, int[WINNER_SIZE], int , int , uint64_t , bool);
void printWinners(int [WINNER_SIZE]);
int pickNext(uint64_t , uint64_t , int winner[WINNER_SIZE],  int x, int y, bool isPlayer, int maxDepth, int pieces, int &retx, int &rety	, bool isX);
int updateWinner(int[WINNER_SIZE]);
int XYToNum(int, int);
void gameLoop(uint64_t , uint64_t , int winner[WINNER_SIZE], bool maximize, int maxDepth, int pieces, bool isX);
bool checkWinner(int [WINNER_SIZE], int);
uint64_t moveToBitboard(int );

#include <iostream>
#include <cstring>
#include <cmath>
#include <time.h>
using namespace std;

int main(){
	uint64_t boardX = 0;
	uint64_t boardY = 0;

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
	//int board[3][3] = {{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}};

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
		gameLoop(boardX, boardY, winner, false, 10, 0, false);
	}else{
		int retx, rety;//    placeholders
		clock_t t = clock();
		for(int i = 0; i < 100; i++)
		pickNext(boardX, boardY, winner, 0, 0, true, 10 , 0, retx, rety, true);

		t = clock() - t;
		cerr << "\n" << ((float)t)/CLOCKS_PER_SEC << endl;
		cout << XYToNum(retx, rety);
		updateBoard(boardX, boardY, winner, retx, rety, 1 << (BOARD_SIZE*retx + rety),true);
		gameLoop(boardX, boardY, winner, true, 10, 1, true);
	}


	//cout << posx << " " << posy << endl;
	//actions[posx][posy] = false;



}


int counter = 0;

uint64_t promptMove( int &x, int & y){
	cerr << "Please make a move: (1-9)" << endl;
	int move;
	cin >> move;
	while(!(abs(move-1) < 9)){
		cerr << "Please make a move: (1-9)" << endl;
		cin >> move;
	}
	uint64_t m = moveToBitboard(move);
	numToXY(move, x, y);
	return m;
}


void gameLoop(uint64_t boardX, uint64_t boardY, int winner[WINNER_SIZE], bool maximize, int maxDepth, int pieces, bool isX){
	int posx, posy;
	uint64_t move = promptMove(posx, posy);
	updateBoard(boardX, boardY, winner, posx, posy, move, !isX);
	//if(DEBUG)(board);
	++pieces;
	if(checkWinner(winner, pieces)){
		return;
	}



	int retx, rety;
	pickNext(boardX, boardY, winner, posx, posy, maximize, maxDepth , pieces, retx, rety, isX);
	cout << XYToNum(retx, rety);
	//if(DEBUG){printBoard(board);}
	uint64_t imove = 1 << (BOARD_SIZE*retx + rety);
	updateBoard(boardX, boardY, winner, retx, rety,imove,isX);
	if(checkWinner(winner, pieces)){
		return;
	}
	++pieces;
	gameLoop(boardX, boardY, winner, maximize, maxDepth, pieces, isX);
}






bool checkWinner(int winner[WINNER_SIZE], int pieces){
	if(pieces >= BOARD_SIZE*BOARD_SIZE){
		cerr << "it's a draw";
	}if(updateWinner(winner) == 1){
			cerr << "I Win";
			return true;
		}else if(updateWinner(winner) == -1){
			cerr << "I Lose";
			return true;
		}else{
			return false;
		}

}


int pickNext(uint64_t boardX, uint64_t boardY, int winner[WINNER_SIZE], int x, int y, bool maximize, int maxDepth, int pieces, int &retx, int &rety, bool isX	){
	//if(DEBUG){
	//cout << ++counter << " maximize: " << maximize << endl;
	//cerr << bitset<32>{boardX} << " \n" << bitset<32>{boardY} << endl;}
	int winnerValue = updateWinner(winner);

	//char c;
	//cin >> c;

	//If weve found a winner or are at max recursion depth, exit the program
	if(winnerValue != 0 || maxDepth == 0 || pieces >= 9)
		return winnerValue;//+heuristic

	//The Minimax part
	int m = (maximize) ? 1 : -1;

	int max = -2;//lower than any possible move

	for(int i = 0; i < BOARD_SIZE; i++){
		for(int j = 0; j < BOARD_SIZE; j++){
			int move = BOARD_SIZE*i + j;
			uint64_t imove = 1 << (move );
			uint64_t actions = boardX | boardY;
			if((actions | imove) != actions){//will never not occur because of pieces variable

				//copy
				int newWinner[WINNER_SIZE];
				memcpy(newWinner, winner, sizeof(int) * (WINNER_SIZE));
				uint64_t newBoardX = boardX; uint64_t newBoardY = boardY;
				updateBoard(newBoardX, newBoardY, newWinner, i, j, imove, isX);
				int retx2, rety2;
				int newMax = m * pickNext( newBoardX, newBoardY, newWinner,i,j,!maximize , maxDepth - 1, pieces+1, retx2, rety2, !isX);

				if(newMax > max){
					max = newMax;
					retx = i;
					rety = j;
				}
			}


		}
	}
	//if(DEBUG){
	//	cout << m*max << ": " << pieces << " " << retx << " " << rety << endl;
		//printBoard(board);
	//}
	return m*max;
}









//
//Update Board and check the terminal condition of a board
//must be done together so that we can optimize out terminal checks to just those affected
//1 = Player win
//0 = draw
//-1 = Enemy win
//null: nonterminal
void updateBoard(uint64_t &boardX, uint64_t &boardY, int winner[WINNER_SIZE], int x, int y, uint64_t move, bool isX){

	if(isX)
		boardX = boardX | move;
	else
		boardY = boardY | move;

	//if(DEBUG)printBoard(board);

	int d = (isX) ? 1 : -1;
	winner[BOARD_SIZE+x]+= d;
	winner[y]+= d;
	if(y==x){
		winner[BOARD_SIZE*2]+= d;
	}
	if(y==0 && x==2 || y == 2 && x == 0 || ( y == 1 && x == 1)){
		winner[BOARD_SIZE*2+1]+= d;
	}
	if(DEBUG)printWinners(winner);

}


//update winner array based on move
//if isPlayer, add, else subtract
int updateWinner(int winner[WINNER_SIZE]){
	for(int i = 0; i < WINNER_SIZE; i++){
		if(winner[i] >= BOARD_SIZE){//X's
			return 1;
		}
		if(winner[i] <= BOARD_SIZE*-1){//O's
			return -1;
		}
	}
	return 0;
}


inline uint64_t moveToBitboard(int move){
	uint64_t ret = 1;
	ret = ret << (move-1);
	return ret;
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
