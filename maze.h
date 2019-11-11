#ifndef classes
#define classes

#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <cmath>


template <typename T> class Point {
        T x_coord;
        T y_coord;

    public:
        Point(T x, T y): x_coord{x}, y_coord{y} {}
        Point(Point <T> & other): x_coord{other.x_coord}, y_coord{other.x_coord} {}
        ~Point() {}
        
        std::pair<T, T> getcoords() {
            std::pair<T,T> retval = std::pair<T,T>{x_coord, y_coord};
            return retval;
        }

        Point <T> operator + (Point <T> & other) {
            Point <T> temp = Point<T>{x_coord, y_coord};
            temp.x_coord = temp.x_coord + other.x_coord;
            temp.y_coord = temp.y_coord + other.y_coord;
            return temp;
        }
        Point <T> operator - (Point <T> & other) {
            Point <T> temp = Point<T>{x_coord, y_coord};
            temp.x_coord = temp.x_coord - other.x_coord;
            temp.y_coord = temp.y_coord - other.y_coord;
            return temp;
        }
        double EucledianDistance(Point <T> & other) {
            T delta_x = (x_coord - other.x_coord);
            T delta_y = (y_coord - other.y_coord);
            double hyp = sqrt((delta_x * delta_x) + (delta_y * delta_y));
            return hyp;
        }
};

class Cell {
    Point <int> centerSquare;

    Cell(Point<int> & other);
    Cell(int x, int y);
    double judgeDistance(Cell & other);
    ~Cell();
};


class Grid {

    std::vector<std::vector<int>> theGrid;
    Grid(int size);
    Grid(int sizex, int sizey);
    void GenerateMaze();
    void ColourCell(Point <int> cell,std::string colour);
    void GetCell(Point <int> cell);
    void generate_Path();
    ~Grid();
};

std::ostream & operator<<(std::ostream & os, const Grid &);

class graphing_algos {

    std::vector <Point <int>> dJikstra(Grid &, 
            Point <int> & start, Point <int> &end);

    std::vector <Point <int>> breathFirst(Grid &, 
            Point <int> & start, Point <int> &end);

    std::vector <Point <int>> Astar(Grid &, 
            Point <int> & start, Point <int> &end);

};
#endif

