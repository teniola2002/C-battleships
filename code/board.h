#ifndef BOARD_H
#define BOARD_H




#define RESET "\033[0m"
#define BLUE "\033[96m"

class Board{
public:
	static const int SIZE = 10;
	char grid[SIZE][SIZE];
	Board();


	void display(const string& label = "") const ;

};


#endif
