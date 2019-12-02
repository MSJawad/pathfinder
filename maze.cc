#include "maze.h"
#include <iostream>
#include <vector>

Point::Point(int x, int y): x_coord{x}, y_coord{y} {}
Point::Point(const Point & other): x_coord{other.x_coord}, y_coord{other.x_coord} {}
Point::~Point() {}

std::pair<int, int> Point::getcoords() {
    std::pair<int,int> retval = std::pair<int,int>{x_coord, y_coord};
    return retval;
}

Point Point::operator + (Point & other) {
    Point temp{x_coord, y_coord};
    temp.x_coord = temp.x_coord + other.x_coord;
    temp.y_coord = temp.y_coord + other.y_coord;
    return temp;
}
Point Point::operator - (Point & other) {
    Point temp{x_coord, y_coord};
    temp.x_coord = temp.x_coord - other.x_coord;
    temp.y_coord = temp.y_coord - other.y_coord;
    return temp;
}
double Point::EucledianDistance(Point & other) {
    int delta_x = (x_coord - other.x_coord);
    int delta_y = (y_coord - other.y_coord);
    double hyp = sqrt((delta_x * delta_x) + (delta_y * delta_y));
    return hyp;
}


Cell::Cell(Point & other, char colour = 'w'): centerSquare{other}, type{colour} {}

Cell::Cell(int x, int y, char colour = 'w'): centerSquare{x,y}, type {colour} {}

Cell::Cell(const Cell & other): centerSquare{other.centerSquare}, type{other.type} {}

Cell::~Cell() {}


double Cell::judgeDistance(Cell & other) {
    return centerSquare.EucledianDistance(other.centerSquare);
}

Grid::Grid(int size) {
    std::vector<std::vector<Cell>> myGrid;
    for (int y = 0; y < size; y++) {
        std::vector <Cell> myRow;
        for (int x = 0; x < size; x++) {
            Cell coordcell{x,y};
            myRow.push_back(coordcell);
        }
        myGrid.push_back(myRow);
    }
}

Grid::Grid(int sizex, int sizey) {
    std::vector<std::vector<Cell>> myGrid;
    for (int y = 0; y < sizey; y++) {
        std::vector <Cell> myRow;
        for (int x = 0; x < sizex; x++) {
            Cell coordcell{x,y};
            myRow.push_back(coordcell);
        }
        myGrid.push_back(myRow);
    }
}

Grid::~Grid() {}


std::ostream & operator<<(std::ostream & os, const binary_heap<int> & heap) {
    
    for (unsigned int i = 0; i < heap.myHeap.size(); i++) {
        os << heap.myHeap[i] << " ";
    }
    return os;
}
