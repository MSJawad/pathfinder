#include "maze.h"
#include <iostream>
#include <vector>

Point::Point(int x, int y): x_coord{x}, y_coord{y} {}
Point::Point(Point & other): x_coord{other.x_coord}, y_coord{other.x_coord} {}
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

double Cell::judgeDistance(Cell & other) {
    return centerSquare.EucledianDistance(other.centerSquare);
}

Grid::Grid(int size) {
    std::vector<std::vector<Cell *>> myGrid;
}

Grid::Grid(int sizex, int sizey) {
    std::vector<std::vector<Cell *>> myGrid;
}

std::ostream & operator<<(std::ostream & os, const binary_heap<int> & heap) {
    
    for (unsigned int i = 0; i < heap.myHeap.size(); i++) {
        os << heap.myHeap[i] << " ";
    }
    os << std::endl;
    return os;
}
