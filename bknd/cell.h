#ifndef cell
#define cell

#include <iostream>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>
#include <cmath>

class Point {
    int x_coord;
    int y_coord;

    public:
        Point(int x, int y);
        Point(const Point & other);
        ~Point();
        
        std::pair<int, int> getcoords();

        std::string str();

        Point operator + (Point & other);
        Point operator - (Point & other);
        bool operator == (Point & other);
        

        double EucledianDistance(Point & other);

};

class Cell {

    
    Point centerSquare;
    char type;

    int scoresofar;

    public:

        Cell(Point& other, char colour);
        Cell(int x, int y, char colour);
        Cell(const Cell &);
        Point get_coords() const;
        void change_colour(char);
        char get_colour() const;
        int get_score() const;
        void set_score(int);
        double judgeDistance(Cell & other);
        ~Cell();
        friend std::ostream & operator <<(std::ostream & os, const Cell & out);
};

std::ostream & operator <<(std::ostream & os, const Cell & out);

#endif
