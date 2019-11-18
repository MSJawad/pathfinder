#ifndef maze
#define maze

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
        Point(Point & other);
        ~Point();
        
        std::pair<int, int> getcoords();

        Point operator + (Point & other);
        Point operator - (Point & other);
        double EucledianDistance(Point & other);

};

class Cell {

    
    Point centerSquare;
    char type;

    public:
        Cell(Point& other, char colour);
        Cell(int x, int y, char colour);
        double judgeDistance(Cell & other);
        ~Cell();
};


class Grid {

    public:

        std::vector<std::vector<Cell *>> theGrid;
        Grid(int size);
        Grid(int sizex, int sizey);
        void GenerateMaze();
        void ColourCell(Point cell,std::string colour);
        void GetCell(Point cell);
        void generate_Path();
        ~Grid();
};

std::ostream & operator<<(std::ostream & os, const Grid &);

class graphing_algos {

    public:

        std::vector <Point> dJikstra(Grid &, 
                Point & start, Point &end);

        std::vector <Point> breathFirst(Grid &, 
                Point & start, Point &end);

        std::vector <Point> Astar(Grid &, 
                Point & start, Point &end);

};

template<typename T> class binary_heap {
    std::vector <T> myHeap;

    // 2n+1 left child
    // 2n+2 right child
    // n-1/2 parent
    //
    public:
    binary_heap(): myHeap{std::vector<T>(0)} {}

    void add(T val) {
        int idx = myHeap.size();
        myHeap.push_back(val);
        while (myHeap[(idx-1)/2] > val && idx != 0) {
            std::swap(myHeap[idx],myHeap[(idx-1)/2]);
            idx = (idx-1)/2;
        }
    }
    void remove_top() {
        int last_node = myHeap.size() - 1;
        std::swap(myHeap[0], myHeap[last_node]);
        int sz = myHeap.size();
        int idx = 0;

        while (((myHeap[idx] > myHeap[(2*idx)+1]) || (myHeap[idx] > myHeap[(2*idx)+2])) && idx < sz) {
            
            if (myHeap[2*idx + 1] < myHeap[2*idx + 2]) {
                std::swap(myHeap[idx], myHeap[2*idx + 1]);
                idx = 2*idx + 1;
            } 
            else {
                std::swap(myHeap[idx], myHeap[2*idx + 2]);
                idx = 2*idx + 2;
            }
        }
    }
    void look_top() {
        return myHeap[0];
    }

    friend std::ostream & operator<<(std::ostream & os, const binary_heap<int> &);
};

#endif

