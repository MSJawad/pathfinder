#include "cell.h"
#include <string>
#include <bits/stdc++.h>


Point::Point(int x, int y): x_coord{x}, y_coord{y} {}
Point::Point(const Point & other): x_coord{other.x_coord}, y_coord{other.x_coord} {}
Point::~Point() {}

std:: string Point::str() {
    return std::to_string(x_coord) + ',' + std::to_string(y_coord);
}


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

bool Point::operator == (Point & other) {
    return ((other.x_coord == x_coord) && (other.y_coord == y_coord));
}

int Point::ManhattanDistance(Point & other) {
    int delta_x = abs(x_coord - other.x_coord);
    int delta_y = abs(y_coord - other.y_coord);
    return (delta_x + delta_y);
}


Cell::Cell(Point & other, char colour = 'w'): centerSquare{other}, type{colour}, scoresofar{INT_MAX}, neighbour{NULL} {}

Cell::Cell(int x, int y, char colour = 'w'): centerSquare{x,y}, type {colour}, scoresofar{INT_MAX}, neighbour{NULL} {}

Cell::Cell(const Cell & other): centerSquare{other.centerSquare}, type{other.type}, scoresofar{other.scoresofar}, neighbour{NULL} {}

void Cell::change_colour(char colour) {
    type = colour;
}

char Cell::get_colour() const {
    return type;
}

int Cell::get_score() const {
    return scoresofar;
}

void Cell::set_score(int new_score) {
    scoresofar = new_score;
}

Cell * Cell::getNeighbour() const {
    return neighbour;
}

void Cell::setNeighbour(Cell * newbour) {
    neighbour = newbour;
}

Point Cell::get_coords() const {
    return centerSquare;
}

std::ostream & operator <<(std::ostream & os, const Cell & out) {
    os << out.get_colour();
    return os;
}

Cell::~Cell() {}


double Cell::judgeDistance(Cell & other) {
    return centerSquare.ManhattanDistance(other.centerSquare);
}
