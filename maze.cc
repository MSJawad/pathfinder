#include "maze.h"

Cell::Cell(Point <int> & other): centerSquare{new Point<int>{other}} {}

Cell::Cell(const Cell & other): centerSquare{new Point<int>{*(other.centerSquare)}} {}

double Cell::operator=(const Cell & other) {


}

