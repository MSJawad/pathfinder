#ifndef PATHFINDER_SEARCH_H
#define PATHFINDER_SEARCH_H

#include <cstdint>
#include <deque>
#include <string>
#include <vector>

#include "grid.h"
#include "min_heap.h"

enum class Algorithm { AStar, Dijkstra, BFS };

const char *algorithm_name(Algorithm algorithm);

enum class CellState : uint8_t { Unseen, Frontier, Visited, Path };

// A shortest-path search that advances one expanded node per step(), so the
// visualizer can draw it as it runs.
//
// A* orders the frontier by steps taken plus the Manhattan distance to the
// goal, Dijkstra by steps taken alone, and BFS uses a FIFO queue. On this
// unit-cost grid all three return a shortest path.
class Search {
    struct Entry {
        int priority;
        int heuristic;
        uint64_t order;
        int cell;
    };
    struct EntryLess {
        bool operator()(const Entry &a, const Entry &b) const;
    };

    const Grid &grid_;
    Point start_;
    Point goal_;
    Algorithm algorithm_;

    std::vector<int> cost_;
    std::vector<int> parent_;
    std::vector<CellState> state_;
    MinHeap<Entry, EntryLess> heap_;
    std::deque<int> queue_;
    uint64_t next_order_ = 0;

    int nodes_expanded_ = 0;
    bool finished_ = false;
    std::vector<Point> path_;

    void add_to_frontier(int cell, int cost, int parent);
    int take_next();

  public:
    // Throws std::invalid_argument if start or goal is a wall or out of bounds.
    Search(const Grid &grid, Point start, Point goal, Algorithm algorithm);

    // Expands the next frontier node. Does nothing once the search is finished.
    void step();

    Algorithm algorithm() const { return algorithm_; }
    bool finished() const { return finished_; }
    bool found() const { return !path_.empty(); }
    int nodes_expanded() const { return nodes_expanded_; }
    // Number of moves on the path found, or -1 when there is none (yet).
    int path_length() const { return static_cast<int>(path_.size()) - 1; }
    const std::vector<Point> &path() const { return path_; }
    CellState state(Point p) const { return state_[grid_.index(p)]; }
};

struct SearchResult {
    Algorithm algorithm;
    int nodes_expanded;
    int path_length;
    std::vector<Point> path;
    double milliseconds;
};

// Runs a search to completion and times it.
SearchResult solve(const Grid &grid, Point start, Point goal, Algorithm algorithm);

#endif
