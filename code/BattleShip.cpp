//============================================================================
// Name        : BattleShip.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include "board.h"
#include "ships.h"
#define GREEN "\033[32m"
#define RED "\033[31m"

// initializing the board
Board:: Board(){
		for (int i=0; i<SIZE;i+=1){
			for(int j=0; j< SIZE ;j+=1){
				grid[i][j]='~';
			}
		}

	}
// display function
void Board::display(const string& label) const {
    if (!label.empty()) {
       cout << label << endl;
    }
    		cout << "   ";
	        for (char col = 'A'; col <= 'J'; ++col) {
	            cout << col << "  ";
	        }
	        cout << endl;
	        for (int i = 0; i < SIZE; i++) {
	            if ( i + 1 < 10) {
	                cout << " " << (i + 1) << " ";
	            } else {
	                cout << (i + 1) << " ";
	            }
	            for (int j = 0; j < SIZE; j++) {
	                if (grid[i][j] == '~') {
	                    cout << BLUE << "~" << RESET << "  ";
	                } else {
	                    cout << grid[i][j] << "  ";
	                }
	            }
	            cout << endl;
	        }
		}
// string to integer conversion
int stringToInt(const string& str) {
    int result = 0;
    for (char c : str) {
        if (c < '0' || c > '9') {
            throw invalid_argument("Invalid character in input");
        }
        result = result * 10 + (c - '0');
    }
    return result;
}
// checks whether the input can have a place on the board
bool parseCoordinate(const string& coord, int& row, int& col) {
    if (coord.length() < 2 || coord.length() > 3)
        return false;
    char colChar = toupper(coord[0]); // allows for inputing lower-case letters
    if (colChar < 'A' || colChar >= 'A' + Board::SIZE)
        return false;
    col = colChar - 'A';
    string rowStr = coord.substr(1);
    // convert row string to integer using stringToInt
    int rowNum;
    try {
        rowNum = stringToInt(rowStr);
    } catch (const invalid_argument&) {
        return false; // invalid input
    }
// validate row number
    if (rowNum < 1 || rowNum > Board::SIZE)
        return false;
 // convert to zero-based index
    row = rowNum - 1;
    return true;
}
// to check if the ship can be placed in a certain position
bool canPlaceShip(const Board& board, int row, int col, int shipSize, char orientation) {
    // checks if ship is horizontal
	if (orientation == 'H') {
		// checks if it can fit horizontally
        if (col + shipSize > Board::SIZE)
            return false;
        // checks if any of the cells are already occupied
        for (int i = 0; i < shipSize; ++i) {
            if (board.grid[row][col + i] != '~')
                return false;
        }
    } else if (orientation == 'V') {

        if (row + shipSize > Board::SIZE)
            return false;
        for (int i = 0; i < shipSize; ++i) {
            if (board.grid[row + i][col] != '~')
                return false;
        }
    } else {
        return false;
    }
    return true;
}

// function to place the ship on the board
void placeShipOnBoard(Board& board, Ship& ship, int row, int col, char orientation) {
	vector<pair<int, int> > coords;
	if (orientation == 'H') {
	    for (int i = 0; i < ship.getSize(); ++i) {
	        board.grid[row][col + i] = 'S'; // Mark the cell as part of the ship
	        coords.push_back(make_pair(row, col + i)); // Add coordinates to the vector
	    }
	} else if (orientation == 'V') {
	    for (int i = 0; i < ship.getSize(); ++i) {
	        board.grid[row + i][col] = 'S'; // Mark the cell as part of the ship
	        coords.push_back(make_pair(row + i, col)); // Add coordinates to the vector
	    }
	}
	ship.placeShip(coords); // Pass the coordinates to the Ship object
}
// checks if all the ships in the ship vector are sunk
bool allShipsSunk(const vector<Ship*>& ships) {
    for (size_t i = 0; i < ships.size(); ++i) {
        if (!ships[i]->isSunk()) { // checks if ship of size i is sunk
        	cout << "A ship is still afloat!" << endl;
            return false;
        }
    }
    cout << "all ships have been sunk" << endl;
    return true;
}
// determine whether a specific coordinate on the game board is occupied by a ship
Ship* getShipAtCoordinate(const vector<Ship*>& ships, int row, int col) {
    for (size_t i = 0; i < ships.size(); ++i) {
        Ship* ship = ships[i];
        const vector<pair<int, int> >& coords = ship->getCoordinates(); // returns a vector which represents the coordinates occupied by the ship
        for (size_t j = 0; j < coords.size(); ++j) { // 	loops through all the coordinates stored in coords
            if (coords[j].first == row && coords[j].second == col) { //if match is found it means the ship occupies the specified position
                return ship;
            }
        }
    }
    return NULL; // if there is no ship at a specific position
}

int main() {
	srand(time(NULL)); // initializes random number generator

	bool aiHuntMode = true; // a.i. starts in hunt mode
	vector<pair<int, int> > aiHitStack; //keep track of hit positions for targeting

	// initialize boards for player and A.I.
    Board playerBoard;
    Board aiBoard;


    // initialize tracking boards for player and A.I.
    Board playerTrackingBoard;
    Board aiTrackingBoard;

    int roundNumber = 1;  // start with round 1
    // initializes a vector of pointers to ship objects
    vector<Ship*> ships;

    // push_back adds each Ship pointer to the ships vector.
    ships.push_back(new Ship("Carrier", 5));
    ships.push_back(new Ship("Battleship", 4));
    ships.push_back(new Ship("Cruiser", 3));
    ships.push_back(new Ship("Submarine", 3));
    ships.push_back(new Ship("Destroyer", 2));

    //player places ships
    for (size_t i = 0; i < ships.size(); ++i) {
        Ship* ship = ships[i];
        bool placed = false; // initializes placed to false
        while (!placed) {
            playerBoard.display("place all your ships (e.g. a7, h10):");
            cout << "Enter the starting coordinate for " << ship->getName() << " (size " << ship->getSize() << "): ";// prompts the player to enter a starting coordinate for current ship.
            string coordInput;
            cin >> coordInput;// user enters coordinates
            int row, col;
            if (!parseCoordinate(coordInput, row, col)) {// parse coordinate function is called to see if inputed coordinates are valid
                cout << "Invalid coordinate. Please enter again (e.g., A1, B5):" << endl;
                continue; // allows player to try again
            }
            cout << "Enter orientation (H for horizontal, V for vertical (ships can only be placed in the right direction or downwards )): "; //
            char orientation;
            cin >> orientation;
            orientation = toupper(orientation);
            if (orientation != 'H' && orientation != 'V') {
                cout << "Invalid orientation. Please enter H or V." << endl;
                continue;
            }
            // checks if ships can be placed
            if (!canPlaceShip(playerBoard, row, col, ship->getSize(), orientation)) {
                cout << "Cannot place ship at that position. It may be overlapping with another ship." << endl;
                continue;
            }


            placeShipOnBoard(playerBoard, *ship, row, col, orientation);
            placed = true;// sets the boolean variable placed to true

        }
    }

    // AI places ships
    vector<Ship*> aiships;
        aiships.push_back(new Ship("Carrier", 5));
        aiships.push_back(new Ship("Battleship", 4));
        aiships.push_back(new Ship("Cruiser", 3));
        aiships.push_back(new Ship("Submarine", 3));
        aiships.push_back(new Ship("Destroyer", 2));

        for (size_t i = 0; i < aiships.size(); ++i) {
            Ship* ship = aiships[i];
            bool placed = false;
            while (!placed) {
            	// randomly selects coordinates
                int row = rand() % Board::SIZE;
                int col = rand() % Board::SIZE;
                // randomly selects the orientation
                char orientations[] = { 'H', 'V' };
                char orientation = orientations[rand() % 2];
                // if ship cannot be placed try again
                if (!canPlaceShip(aiBoard, row, col, ship->getSize(), orientation)) {
                    continue;
                }
                placeShipOnBoard(aiBoard, *ship, row, col, orientation);
                placed = true;
            }
        }
    // only displays players board so player doesn't know where AI's board is
    playerBoard.display("Your Board : ");


    // Initialize game over
    bool gameOver = false;

    // game loop
    while (gameOver == false) { // game will continue until game over == true
    	cout << "\n--- Round " << roundNumber << " ---" << endl; // display round number
        // player's turn
    	bool playerTurn = true;
    	while(playerTurn){
        cout << "\nYour Tracking Board:" << endl;
        playerTrackingBoard.display();// displays player's tracking board
        cout << "Enter coordinate to fire at: ";
        string coordInput;
        cin >> coordInput;
        int row, col;
        // checks validity of coordinates
        if (!parseCoordinate(coordInput, row, col)) {
            cout << "Invalid coordinate. Please enter again (e.g., A1, B5):" << endl;
            continue;
        }
        if (playerTrackingBoard.grid[row][col] != '~') { // checks if cell has been fired at
            cout << "You have already fired at this coordinate. Please choose another one." << endl;
            continue;
        }
        // registers hits
        Ship* hitShip = getShipAtCoordinate(aiships, row, col);
        if (hitShip != NULL) {
            // hit
            cout << GREEN << "Hit!"<< RESET << endl;
            hitShip->registerHit(); // tracks the number of times a ship has been hit
            //both boards will show if hits were successful
            playerTrackingBoard.grid[row][col] = 'X';
            aiBoard.grid[row][col] = 'X';
            if (hitShip->isSunk()) {
                cout << "You sunk the " << hitShip->getName() << "!" << endl;
            }
        } else {
            // miss
            cout << RED << "Miss!" << RESET << endl;
            playerTrackingBoard.grid[row][col] = 'O';
            aiBoard.grid[row][col] = 'O';
            playerTurn = false; // end the players turn
        }
        // check if player won
        if (allShipsSunk(aiships)) {
            cout << "Congratulations! You have sunk all of A. Smith's ships. You win!" << endl;
            gameOver = true;
            break;
        }
   }
        // AI's turn
    	bool aiTurn =true;
    	while (aiTurn){
        cout << "\nA. Smith is taking a shot..." << endl; // tells player AI is making a move
        int aiRow, aiCol;
        if (aiHuntMode){
        do {
            aiRow = rand() % Board::SIZE;
            aiCol = rand() % Board::SIZE;
        } while (aiTrackingBoard.grid[aiRow][aiCol] != '~');
        }else{
        	// target mode
        	if(!aiHitStack.empty()){
        		pair<int, int > target = aiHitStack.back();
        		aiHitStack.pop_back();
                aiRow = target.first;
                aiCol = target.second;

                // Skip if already targeted
                if (aiTrackingBoard.grid[aiRow][aiCol] != '~') {
                    continue;
                }
            } else {
                // No more positions to target; switch back to hunt mode
                aiHuntMode = true;
                continue;

            }

        }

        // Check if the coordinate is valid
        if (aiRow < 0 || aiRow >= Board::SIZE || aiCol < 0 || aiCol >= Board::SIZE) {
            continue; // Invalid coordinate, skip and retry
        	}

        Ship* playerShipHit = getShipAtCoordinate(ships, aiRow, aiCol);
        if (playerShipHit != NULL) {
            // Hit
            cout << "A. Smith fires at " << char('A' + aiCol) << aiRow + 1 << " and hits your ship!" << endl;
            playerShipHit->registerHit();
            aiTrackingBoard.grid[aiRow][aiCol] = 'X';
            playerBoard.grid[aiRow][aiCol] = 'X';

            if (playerShipHit->isSunk()) {
                // Ship is sunk
                cout << "A. Smith sunk your " << playerShipHit->getName() << "!" << endl;
                aiHuntMode = true; // Switch back to hunt mode
                aiHitStack.clear(); // Clear the stack since the ship is sunk
            } else {
                // Ship isn't sunk, so switch to target mode
                aiHuntMode = false;

                // Add adjacent cells to the stack
                // Up
                if (aiRow - 1 >= 0 && aiTrackingBoard.grid[aiRow - 1][aiCol] == '~')
                    aiHitStack.push_back(make_pair(aiRow - 1, aiCol));
                // Down
                if (aiRow + 1 < Board::SIZE && aiTrackingBoard.grid[aiRow + 1][aiCol] == '~')
                    aiHitStack.push_back(make_pair(aiRow + 1, aiCol));
                // Left
                if (aiCol - 1 >= 0 && aiTrackingBoard.grid[aiRow][aiCol - 1] == '~')
                    aiHitStack.push_back(make_pair(aiRow, aiCol - 1));
                // Right
                if (aiCol + 1 < Board::SIZE && aiTrackingBoard.grid[aiRow][aiCol + 1] == '~')
                    aiHitStack.push_back(make_pair(aiRow, aiCol + 1));
            }
        } else {
            // Miss
            cout << "A. Smith fires at " << char('A' + aiCol) << aiRow + 1 << " and misses." << endl;
            aiTrackingBoard.grid[aiRow][aiCol] = 'O';
            playerBoard.grid[aiRow][aiCol] = 'O';
            aiTurn = false; // End the AI's turn
        }
        // check if AI won
        if (allShipsSunk(ships)) {
            cout << "All your ships have been sunk. A. Smith wins." << endl;
            gameOver = true;
            break;
        }

  }
    	if (gameOver) break;
    	        roundNumber++; // increments round number
        // display player's board after AI's turn
        cout << "\nYour Board:" << endl;
        playerBoard.display();
    }




    // clean up dynamically allocated memory
    for (size_t i = 0; i < ships.size(); ++i) {
        delete ships[i];
    }
    for (size_t i = 0; i < aiships.size(); ++i) {
            delete aiships[i];
    }

    return 0;
}

