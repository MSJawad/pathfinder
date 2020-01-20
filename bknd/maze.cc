#include "maze.h"
#include <boost/functional/hash.hpp>
#include <cstdlib>
#include <string>
#include <unordered_set>
#include "cell.h"
#include <random>
#include <time.h>
#include <utility>
#include <vector>


Grid::Grid(int size): dims{size,size} {
    for (int y = 0; y < size; y++) {
        std::vector <Cell> myRow;
        for (int x = 0; x < size; x++) {
            Cell coordcell{x,y,'b'};
            myRow.push_back(coordcell);
        }
        theGrid.push_back(myRow);
    }
}

Grid::Grid(int sizex, int sizey): dims{sizex,sizey} {
    for (int y = 0; y < sizey; y++) {
        std::vector <Cell> myRow;
        for (int x = 0; x < sizex; x++) {
            Cell coordcell{x,y,'b'};
            myRow.push_back(coordcell);
        }
        theGrid.push_back(myRow);
    }
}

void Grid::ColourCell(std::pair<int,int> & coordinates, char colour) {
    theGrid[coordinates.second][coordinates.first].change_colour(colour);
}

Cell Grid::GetCell(int x, int y) {
    return theGrid[y][x];
}

std::ostream & operator <<(std::ostream & os, const Grid & out) {
    for (auto & row: out.theGrid) {
        for (auto & elem: row) {
            os << elem;
        }
        os << std::endl; 
    }
    return os;
}


void recursivePrims(Grid & curGrid, std::unordered_set<std::pair<int,int>,
        boost::hash<std::pair<int,int>>> & allfrontiers, std::pair<int,int> & pos, 
        std::pair<int,int> & maxdims) {
    curGrid.ColourCell(pos, 'w');

    bool madepath = false;
    std::vector <std::pair<int,int>> nsew{{0, 2},{0,-2},{2,0},{-2,0}};

    for (auto directions : nsew) {
        std::pair<int,int> nbrs{pos.first+directions.first,
            pos.second+directions.second};
        if ((nbrs.first >= 0) && (nbrs.first < maxdims.first) &&
                (nbrs.second >= 0) && (nbrs.second < maxdims.second)) { 
            auto nbrcell = curGrid.GetCell(nbrs.first, nbrs.second);
            if (nbrcell.get_colour() == 'b') { 
                allfrontiers.insert(nbrs);
                curGrid.ColourCell(nbrs, 'p');
            } else if (nbrcell.get_colour() == 'w') {
                if (!madepath) {
                    madepath = true;
                    std::pair<int,int> mypath{(nbrs.first + pos.first)/2, 
                        (nbrs.second + pos.second)/2};
                    curGrid.ColourCell(mypath,'w');
                }
            }
        }
    }
    if (!allfrontiers.empty()) {
        auto nextpos = std::next(allfrontiers.begin(), 
                    rand() % allfrontiers.size());
        auto val = *(nextpos);
        allfrontiers.erase(nextpos);
        recursivePrims(curGrid, allfrontiers, val, maxdims);
    }

}


void Grid::GenerateMaze() {
    std::srand(time(0));
    std::unordered_set<std::pair<int,int>, 
        boost::hash<std::pair<int,int>>> frontiers;
    std::pair<int,int> start{rand() % dims.first, rand() % dims.second};

    recursivePrims(*this, frontiers, start, dims);
}


Grid::~Grid() {}

std::ostream & operator<<(std::ostream & os, const binary_heap<int> & heap) {
    
    for (unsigned int i = 0; i < heap.myHeap.size(); i++) {
        os << heap.myHeap[i] << " ";
    }
    return os;
}


/* std::vector<Point> graphing_algos::Astar(Grid & graph, Point & start, Point & end) { */

/* } */
