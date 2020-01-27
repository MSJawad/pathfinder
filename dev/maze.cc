#include "maze.h"
#include <boost/functional/hash.hpp>
#include <cstdlib>
#include <string>
#include <unordered_set>
#include "cell.h"
#include <time.h>
#include <vector>

int scale = 700;

Grid::Grid(int size): 
    display{new Xwindow{(scale / size) * size,(scale / size) * size}},
    dims{size,size} {
    for (int y = 0; y < size; y++) {
        std::vector <Cell> myRow;
        for (int x = 0; x < size; x++) {
            Cell coordcell{x,y,'b'};
            myRow.push_back(coordcell);
        }
        theGrid.push_back(myRow);
    }
    display->fillRectangle(0,0, (scale / size) * size, 
            (scale / size) * size,display->Black);
}


Grid::Grid(int sizex, int sizey): 
    display{new Xwindow{(scale / sizex) * sizex,(scale / sizey) * sizey}}, 
    dims{sizex,sizey} {
    for (int y = 0; y < sizey; y++) {
        std::vector <Cell> myRow;
        for (int x = 0; x < sizex; x++) {
            Cell coordcell{x,y,'b'};
            myRow.push_back(coordcell);
        }
        theGrid.push_back(myRow);
    }
    display->fillRectangle(0,0, (scale / sizex) * sizex, 
            (scale / sizey) * sizey,display->Black);
}

void Grid::ColourCell(std::pair<int,int> & coordinates, char colour) {
    theGrid[coordinates.second][coordinates.first].change_colour(colour);
    std::pair<int,int> cell_size{scale / dims.first,scale / dims.second};

    if (colour == 'w') {
        display->fillRectangle(coordinates.first*cell_size.first,
                coordinates.second*cell_size.second,
                cell_size.first,cell_size.second,display->White);

    } else if (colour == 'p') {
        display->fillRectangle(coordinates.first*cell_size.first,
                coordinates.second*cell_size.second,cell_size.first,
                cell_size.second,display->Salmon);
    } else {
        display->fillRectangle(coordinates.first*cell_size.first,
                coordinates.second*cell_size.second,cell_size.first,
                cell_size.second,display->Black);
    }
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
    int num1 = rand() % dims.first;
    int num2 = rand() % dims.second;
    num1 = num1 - (num1 % 3);
    num2 = num2 - (num2 % 3);

    std::pair<int,int> start{num1, num2};

    recursivePrims(*this, frontiers, start, dims);
}


Grid::~Grid() {
    delete display;
}

std::ostream & operator<<(std::ostream & os, const binary_heap<int> & heap) {
    
    for (unsigned int i = 0; i < heap.myHeap.size(); i++) {
        os << heap.myHeap[i] << " ";
    }
    return os;
}


/* std::vector<Point> graphing_algos::Astar(Grid & graph, Point & start, Point & end) { */

/* } */
