#include <stdio.h>


/*State: 2D array of board
initiative

Actions: avaiable spots to place x's and o's

Transition Model: update board state

Initial State: Empty Board, x goes first

Goal State: End when 3 in a row, Horiz, Vert, Diag
			No Empty Squares + no Goal = draw

*/

const int BOARD_SIZE = 3;
void printBoard(int[BOARD_SIZE][BOARD_SIZE]);
#define TRUE 0;
#define FALSE 1;

int main(int argc, char* argv){

	int x = 0;
	printf("%d", x);
	printf("%d", x+1);
	printf("%d", x+2);

	//Array of Board, Initialized to Blank
	//-1 = Empty
	// 0 = "0"
	// 1 = "X"
	int board[3][3] = {{-1,-1,-1},{-1,-1,-1},{1,-1,-1}};

	//Array of Places on Board Still Open
	int actions[3][3] = {{TRUE,TRUE,TRUE},{TRUE,TRUE,TRUE}, {TRUE,TRUE,TRUE}};
	int b = TRUE;

	printf("hello, world\n\n\n");
	printBoard(board);
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