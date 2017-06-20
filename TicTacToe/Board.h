
class Board {
  public:
    int board [3][3] = {{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}};;
    int winner[8] = {0};
    int pieces = 0;
    int hSum = 0;
    Board();
    ~Board();
};
