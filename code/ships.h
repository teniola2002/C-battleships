
#ifndef SHIP_H
#define SHIP_H

#include <vector>
#include <string>
#include <utility> // For std::pair

class Ship {
private:
    std::string name;                     // Name of the ship
    int shipSize;                         // Size of the ship
    int hits;                             // Number of hits
    std::vector<std::pair<int, int> > coordinates; // Coordinates occupied by the ship

public:
    // changed parameter name to 'shipSizeParam' to avoid conflicts
    Ship(const std::string& name, int shipSizeParam) : name(name), shipSize(shipSizeParam), hits(0) {}

    int getSize() const { return shipSize; }

    // get the name of the ship
    const std::string& getName() const { return name; }

    // place the ship at specific coordinates
    void placeShip(const std::vector<std::pair<int, int> >& coords) {
        coordinates = coords;
    }
    // increments hits counter
    void registerHit() {hits++;}

    // compares size of ship to hits to see if it's sunk
    bool isSunk() const {return hits >= shipSize;}

    // to access ship's coordinates
    const std::vector<std::pair<int, int> >& getCoordinates() const { return coordinates; }


};

#endif





