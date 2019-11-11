#include "maze.h"
#include <vector>

Cell::Cell(Point <int> & other): centerSquare{other} {}

Cell::Cell(int x, int y): centerSquare{x,y} {}

double Cell::judgeDistance(Cell & other) {
    return centerSquare.EucledianDistance(other.centerSquare);
}

Grid::Grid(int size): theGrid{std::vector<std::vector<int>>
    (size,std::vector<int>(size,0))} {}

Grid::Grid(int sizex, int sizey): theGrid{std::vector<std::vector<int>> (sizey,std::vector<int>(sizex,0))} {}


