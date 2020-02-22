#include "maze.h"
#include <boost/functional/hash.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_set>
#include "cell.h"
#include <time.h>
#include <vector>
#include <unistd.h>

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
    display->fillRectangle(0,0, (scale / sizex) * sizex,(scale / sizey) * sizey,display->Black);
}

void Grid::ColourCell(std::pair<int,int> & coordinates, char colour) {
    theGrid[coordinates.second][coordinates.first].change_colour(colour);
    std::pair<int,int> cell_size{scale / dims.first,scale / dims.second};

    auto col = display->Black;

    switch (colour) {
        case 'w':
            col = display->White;
            break;
        case 'p':
            col = display->Salmon;
            break;
        case 'g':
            col = display->Green;
            break;
        case 'c':
            col = display->Cyan;
            break;
        case 'm':
            col = display->Magenta;
            break;
        case 'k':
            col = display->Khaki;
            break;
        case 'l':
            col = display->Blue;
            break;
        default:
            col = display->Black;
            break;
    }

    
    display->fillRectangle(coordinates.first*cell_size.first,
            coordinates.second*cell_size.second,cell_size.first,
            cell_size.second,col);
}

Cell Grid::GetCell(std::pair<int,int> & coords) {
    return theGrid[coords.second][coords.first];
}

std::pair <int,int> Grid::getmaxdims() {
    return dims; 
}

int Grid::getCellScore(std::pair<int,int> & coords) {
    return (this->theGrid)[coords.second][coords.first].get_score();
}

void Grid::setCellScore(std::pair<int,int> & coords, int score) {
    (this->theGrid)[coords.second][coords.first].set_score(score);
}

std::pair<int,int> Grid::getCellNeighbour(std::pair<int,int> & coords) {
    std::pair<int,int> nbr = 
        ((this->theGrid)[coords.second][coords.first]).getNeighbour();
    if (nbr.first >= 0) {
        std::pair<int, int> retval = nbr; 
        return retval; 
    } else {
        std::pair<int,int> invalid{-1,-1};
        return invalid;
    }
}

void Grid::setCellNeighbour(std::pair<int,int> & coords, 
        std::pair<int,int> & nbrcoords) {
    ((this->theGrid)[coords.second][coords.first]).setNeighbour(nbrcoords);
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
            auto nbrcell = curGrid.GetCell(nbrs);
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

class pair_wrapper {
    std::pair<int,int> rawclass;
    int heurStart;
    int heurEnd;
    public:
        pair_wrapper() {}

        pair_wrapper(std::pair<int,int>& curr, int stepssofar, Point & end):
            rawclass{curr}, heurStart{stepssofar} {
            Point temp{curr.first,curr.second};
            heurEnd = temp.ManhattanDistance(end);
        }
        bool operator <(pair_wrapper & other) {
            return (heurEnd+heurStart) < (other.heurEnd + other.heurStart);
        }
        bool operator ==(pair_wrapper & other) {
            return (heurEnd+heurStart) == (other.heurEnd + other.heurStart);
        }
        bool operator >(pair_wrapper & other) {
            return (heurEnd+heurStart) > (other.heurEnd + other.heurStart);
        }
        std::pair<int,int> retcoords() {
            return rawclass;
        }
        int getscore() {
            return heurEnd + heurStart;
        }
        int get_starth() {
            return heurStart;
        }
        int get_endh() {
            return heurEnd;
        }

        ~pair_wrapper() {}
};

void graphing_algos::Astar(Grid & graph, Point & start, Point & end) { 
    std::pair<int,int> curr = start.getcoords();
    std::pair<int,int> endcond = end.getcoords();

    if (graph.GetCell(curr).get_colour() == 'b') {
        throw "cant start at black";
    } else if (graph.GetCell(endcond).get_colour() == 'b') {
        throw "cant end at black";
    } else {
        graph.ColourCell(curr,'l');
        graph.ColourCell(endcond,'l');
    }

    std::unordered_set<std::pair<int,int>,
        boost::hash<std::pair<int,int>>> open_nodes{start.getcoords()};

    std::unordered_set<std::pair<int,int>, 
        boost::hash<std::pair<int,int>>> closed_nodes;
    
    binary_heap<pair_wrapper> track;

    std::vector <std::pair<int,int>> nsew{{0, 1},{0,-1},{1,0},{-1,0}};
    int steps = 0;

    std::pair<int,int> maxdims = graph.getmaxdims();

    while (curr != endcond) {
        usleep(1000000);
        
        if (open_nodes.find(curr) == open_nodes.end()) {
            throw "curr not in open nodes";
        }
        open_nodes.erase(curr);
        if (closed_nodes.find(curr) != closed_nodes.end()) {
            throw "curr in closed nodes";
        }
        closed_nodes.insert(curr);
        
        std::vector <std::pair<int,int>> neighbours;
        steps++;
        for (auto neighbour: nsew) {
        
            std::pair<int,int> nbrs{neighbour.first+curr.first,
                neighbour.second+curr.second};
            if (!((nbrs.first >= 0) && (nbrs.first < maxdims.first) &&
                    (nbrs.second >= 0) && (nbrs.second < maxdims.second))) { 
            continue;
            }
            else if ((graph.GetCell(nbrs).get_colour() != 'b') &&
                    (closed_nodes.find(nbrs) == closed_nodes.end())) {
                neighbours.push_back(nbrs);
            }
        }

        for (auto nbr: neighbours) {
            graph.ColourCell(nbr,'g');
            open_nodes.insert(nbr);
            int score = graph.getCellScore(nbr);
            
            pair_wrapper nbrwrp{nbr,steps, end}; 
            if (score == INT_MAX) {
                track.add(nbrwrp);
                graph.setCellScore(nbr,nbrwrp.getscore());
                graph.setCellNeighbour(nbr, curr);
                std::cout << "("<< nbr.first <<  ", "<<nbr.second << 
                    ")'parent is set as ("<< graph.getCellNeighbour(nbr).first <<
                    ", "<<graph.getCellNeighbour(nbr).second << ")"
                    << std::endl;
            } else if (std::min(nbrwrp.getscore(), score) == nbrwrp.getscore()) {
                track.add(nbrwrp);
                graph.setCellScore(nbr,nbrwrp.getscore());
                graph.setCellNeighbour(nbr, curr);
            }
        }

        while (true) {
            if (track.is_Empty()) {
                throw "b_heap is empty";
            }
            pair_wrapper wrpnxt = track.look_top();
            std::pair<int,int> pos_next = wrpnxt.retcoords();
            if (closed_nodes.find(pos_next) != closed_nodes.end()) {
                track.remove_top();
                continue;
            } else {
                track.remove_top();
                curr = pos_next;
                steps = wrpnxt.get_starth();
                break;
            }

        }
    }
    std::pair <int, int> pos = end.getcoords();
    pos = graph.getCellNeighbour(pos);

    while (pos.first >= 0) {
        graph.ColourCell(pos,'c');
        pos = graph.getCellNeighbour(pos);
    }
    curr = start.getcoords();
    graph.ColourCell(curr,'l');
    graph.ColourCell(endcond,'l');
}
