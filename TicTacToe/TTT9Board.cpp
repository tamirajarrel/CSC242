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
#include "TTT9Board.h"
#include <iostream>
#include <cstring>
#include <cmath>
#include <time.h>
#include <algorithm>

using namespace std;

const int DEBUG = false;
const int TIMING = true;


int main(){
	uint64_t xBoard = 0;
	uint64_t oBoard = 0;
	setbuf(stdout, NULL);
	setvbuf (stdout, NULL, _IONBF, BUFSIZ);

	//cerr << "Pick a side: ('X' or 'O' )" << "\n";
	char side;
	cin >> side;
	while(side != 'X' && side != 'O' && side != 'x' && side!= 'o'){
		cerr << "Please, pick a side: ('X' or 'O') " << "\n";
		cin >> side;
	}

	//cerr << "You have chosen " << ((side == 'x' || side == 'X') ? "X" : "O") << " as your side." << "\n";

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
	//int winner[BOARD_SIZE*2 + 2] = {0};


	/*Board boardList[BOARD_SIZE][BOARD_SIZE];
	for(int i = 0; i < BOARD_SIZE; i++){
		for(int j = 0; j < BOARD_SIZE; j++){
			boardList[i][j] = Board();
		}
	}*/

	Board boardList[BOARD_SIZE*BOARD_SIZE];
	for(int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++ ){
		boardList[i] = Board();
	}

	int posx, posy;
	bool maximize = false;

	if(side == 'o' || side == 'O'){
		gameLoop(boardList, false, 13, 0, false, 0, -1);//maxdepth = 13
	}else{
		//int retx, rety;//    placeholders
		//clock_t t = clock();
		//for(int i = 0; i < 100; i++)
		//int retBoard;
		//pickNext(boardList, 0, 0, true, 10 , 0, retx, rety, true, -3000, 3000 , 0, retBoard, 0, 5);
		//t = clock() - t;
		//cerr << "\n" << ((float)t)/CLOCKS_PER_SEC << "\n";
		cout << 5 << " " << 5 << endl;
		updateBoard(boardList[4], 1, 1, true);
		gameLoop(boardList, true, 13, 1, true, 4, 5);//4, 5 are hardwired from move 5 5
	}


	//cout << posx << " " << posy << "\n";
	//actions[posx][posy] = false;



}


int counter = 0;

void promptMove( int &boardNum, int &x, int & y, Board boardList[BOARD_SIZE*BOARD_SIZE], int nextPlay){
	cerr << "Please pick a board and a move: (1-9)" << "\n";
	int board;
	int move;
	cin >> board;
	cin >> move;

	int retx, rety;
	numToXY(move, retx, rety);

	while((!(abs(move-1) < 9)) || (!(abs(board-1) < 9)) ||  (nextPlay != -1 && nextPlay != board) || boardList[board-1].board[retx][rety] != -1 ){


		cerr << "Invalid: Please pick a board and a move: (1-9)" << "\n";
		cin >> board;
		cin >> move;
		numToXY(move, retx, rety);
	}
	numToXY(move, x, y);
	boardNum = board - 1;
}


void gameLoop(Board boardList[BOARD_SIZE*BOARD_SIZE], bool maximize, int maxDepth, int totalPieces, bool isX, int heuristicSum, int nextPlay){
	int posx, posy, boardNum;
	promptMove(boardNum,posx, posy, boardList, nextPlay);
	heuristicSum+=updateBoard(boardList[boardNum], posx, posy, !isX);
	boardList[boardNum].pieces++;
	++totalPieces;
	if(checkWinner(boardList[boardNum], totalPieces)){
		return;
	}


	int nextBoard = XYToNum(posx, posy) - 1;
	int retx, rety, retBoard;
	pickNext(boardList, posx, posy, maximize, maxDepth , totalPieces, retx, rety, isX, -3000, 3000, nextBoard, retBoard, heuristicSum, 5);
	cout  << nextBoard+1 << " " << XYToNum(retx, rety) << endl;
	cerr << "\n";
	heuristicSum+=updateBoard(boardList[nextBoard], retx, rety, isX);
	if(DEBUG){cerr<<"\n";}
	 printBigBoard(boardList);//////////////
	++totalPieces;
	++boardList[nextBoard].pieces;
	if(checkWinner(boardList[nextBoard], totalPieces)){
		return;
	}

	gameLoop(boardList, maximize, maxDepth, totalPieces, isX,heuristicSum, XYToNum(retx, rety));
}






bool checkWinner(Board & b, int totalPieces){
	if(updateWinner(b) == 1000){
			cerr << "X Wins";
			return true;
		}else if(updateWinner(b) == -1000){
			cerr << "O Wins";
			return true;
		}else if(totalPieces >= 81){
			cerr << "it's a draw";
		}else{
			return false;
		}

}


int pickNext(Board boardList[BOARD_SIZE*BOARD_SIZE], int x, int y, bool maximize, int maxDepth, int totalPieces, int &retx, int &rety, bool isX, int alpha, int beta, int boardPick, int & retBoard, int heuristicSum, int prevBoard	){
	if(DEBUG)
	cerr << ++counter << " maximize: " << maximize << "\n";
	int winnerValue = updateWinner(boardList[prevBoard]);

	//char c;
	//cin >> c;

	//If weve found a winner or are at max recursion depth, exit the program
	if(winnerValue != 0 || maxDepth == 0 || totalPieces >= 81){
		if(winnerValue!=0 || totalPieces >= 81) return winnerValue;//+heuristic
		else return heuristicSum;
	}

	//The Minimax part
	int m = (maximize) ? 1 : -1;

	int scoreMax = -2000;//lower than any possible move
	int oldH = heuristicSum;
	for(int i = 0; i < BOARD_SIZE; i++){
		for(int j = 0; j < BOARD_SIZE; j++){

			if(boardList[boardPick].board[i][j] == -1){//will never not occur because of pieces variable
				if(DEBUG){
					cerr << boardPick << "\n";
				}
				int newHSum= oldH+updateBoard(boardList[boardPick], i, j, isX);
				int retx2, rety2, retBoard2;
				int newBoard =  XYToNum(i,j) - 1;
				int newMax = m * pickNext( boardList, i,j,!maximize , maxDepth - 1, totalPieces+1, retx2, rety2, !isX, alpha, beta, newBoard, retBoard2 , newHSum, boardPick);

				undoBoard(boardList[boardPick], i, j , isX);

				if(newMax > scoreMax){
					scoreMax = newMax;
					retx = i;
					rety = j;
					retBoard = newBoard;
					heuristicSum = newHSum;
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
		cerr << "score: " <<  m*scoreMax << " heuristic: " << heuristicSum << " pieces: " << totalPieces << " depth: " << maxDepth << " retx: " << retx << " rety: " << rety << " retboard: " << retBoard << " allpha: " << alpha << " beta: " << beta << "\n";
		printBigBoard(boardList);
		//char c;
		//cin >> c;
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
const int factor = 3;
int updateBoard(Board & b, int x, int y, bool isX){
	b.board[x][y] = (isX) ? 1 : 0;
	int hTotal;
	//if(DEBUG)printBoard(b.board);

	int htotal = 0;
	int d = (isX) ? 1 : -1;
	int total = 0;
	bool flagA= false;bool flagB = false; bool flagC  = false;bool flagD = false;

	if(b.winner[3+x] * d == -2){
		total += factor*d;
		flagA = true;
	}
	b.winner[3+x]+= d;
	if(b.winner[3+x] == 2*d){
		total += factor*d;
		flagA = true;
	}
	if(!flagA)
		total += d;


	if(b.winner[y] * d == -2){
		total += factor*d;
		flagB = true;
	}
	b.winner[y]+= d;
	if(b.winner[y] == 2*d){
		total += factor*d;
		flagB = true;
	}
	if(!flagB)
		total+=d;



	if(y==x){
		if(b.winner[BOARD_SIZE*2] * d == -2){
			total += factor*d;
			flagC = true;
		}
		b.winner[BOARD_SIZE*2]+= d;
		if(b.winner[BOARD_SIZE*2] == 2*d){
			total += factor*d;
			flagC = true;
		}
		if(!flagC)
		total+=d;
	}
	if(y==0 && x==2 || y == 2 && x == 0 || ( y == 1 && x == 1)){
		if(b.winner[BOARD_SIZE*2+1] * d == -2){
			total += factor*d;
			flagD = true;
		}
		b.winner[BOARD_SIZE*2+1]+= d;
		if(b.winner[BOARD_SIZE*2+1] == 2*d){
			total += factor*d;
			flagD = true;
		}
		if(!flagD)
		total+=d;
	}
	//if(DEBUG)printWinners(b.winner);
	b.hSum+=total;
	return total;

}
/*
int updateBoard(Board & b, int x, int y, bool isX){
	b.board[x][y] = (isX) ? 1 : 0;
	int hTotal;
	if(DEBUG)printBoard(b.board);
	int htotal = 0;
	int d = (isX) ? 1 : -1;
	int total = 0;
	b.winner[3+x]+= d;
	if(b.winner[3+x] == 2){
		hTotal += 2;
	}
	b.winner[y]+= d;
	total += 2*d;
	if(y==x){
		b.winner[BOARD_SIZE*2]+= d;
		total+=d;
	}
	if(y==0 && x==2 || y == 2 && x == 0 || ( y == 1 && x == 1)){
		b.winner[BOARD_SIZE*2+1]+= d;
		total+=d;
	}
	if(DEBUG)printWinners(b.winner);
	b.hSum+=total;
	return total;

}*/








void undoBoard(Board & b, int x, int y, bool isX){
	b.board[x][y] = -1;

	int total = 0;
	int d = (isX) ? -1 : 1;
	b.winner[3+x]+= d;
	b.winner[y]+= d;
	total +=2*d;
	if(y==x){
		b.winner[BOARD_SIZE*2]+= d;
		total+=d;
	}
	if(y==0 && x==2 || y == 2 && x == 0 || ( y == 1 && x == 1)){
		b.winner[BOARD_SIZE*2+1]+= d;
		total+=d;
	}
	b.hSum += total;
}


//update winner array based on move
//if isPlayer, add, else subtract
int updateWinner(Board & b){
	for(int i = 0; i < WINNER_SIZE; i++){
		if(b.winner[i] >= 3){//X's
			return 1000;
		}
		if(b.winner[i] <= -3){//O's
			return -1000;
		}
	}
	return 0;
}



void printBigBoard(Board boardList[BOARD_SIZE*BOARD_SIZE]){

		for(int row = 0; row < 3; row++ ){
			for(int i= 0 ; i < 3; i++){
				for(int col = 0 ; col < 3; col++){
					if(boardList[i].board[row][col] == 1){
						cerr << "X" << " ";
					}else if(boardList[i].board[row][col] == 0){
						cerr << "O" << " ";
					}else{
						cerr << "_" << " ";
					}
				}
				cerr << " ";
			}
			cerr << "\n";
		}

		cerr << "\n";
		for(int row = 0; row < 3; row++ ){
			for(int i= 3 ; i < 6; i++){
				for(int col = 0 ; col < 3; col++){
					if(boardList[i].board[row][col] == 1){
						cerr << "X" << " ";
					}else if(boardList[i].board[row][col] == 0){
						cerr << "O" << " ";
					}else{
						cerr << "_" << " ";
					}
				}
				cerr << " ";
			}
			cerr << "\n";
		}

		cerr << "\n";

		for(int row = 0; row < 3; row++ ){
			for(int i= 6 ; i < 9; i++){
				for(int col = 0 ; col < 3; col++){
					if(boardList[i].board[row][col] == 1){
						cerr << "X" << " ";
					}else if(boardList[i].board[row][col] == 0){
						cerr << "O" << " ";
					}else{
						cerr << "_" << " ";
					}				}
				cerr << " ";
			}
			cerr << "\n";
		}

		cerr << "\n";
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
	cerr << "\n";
	for(int i = 0; i < WINNER_SIZE; i++){
		cerr << " " << w[i] << " " ;
	}
	cerr << "\n";
}
