Ben King
bking11@u.rochester.edu
I collaborate on this project and all work is my own.


Files:
  3x3:
    TTT.cpp
    TTT.h

  9x9:
    TTT9Board.cpp
    TTT9Board.h
    Board.cpp
    Board.h

Compilation & Run Instructions:
  3x3:
    g++ -o TTT.out -std=c++11 TTT.cpp   //Compilation
    ./TTT.out                           //Run
  9x9:
    g++ -o TTT9Board.out -std=c++11 TTT9Board.cpp Board.cpp     //Compilation
    ./TTT9Board.out                                             //Run

Input Format:
  3x3:
    Input first X or O for the player's choice, and then Input each move 1-9 as prompted

  9x9:
    Input first X or O for the computer's choice, and then Input each move in tournament format
