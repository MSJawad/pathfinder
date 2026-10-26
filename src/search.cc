#include "search.h"

#include <algorithm>
#include <chrono>
#include <climits>
#include <stdexcept>

const char *algorithm_name(Algorithm algorithm) {
    switch (algorithm) {
    case Algorithm::AStar:
        return "A*";
    case Algorithm::Dijkstra:
        return "Dijkstra";
    case Algorithm::BFS:
        return "BFS";
    }
    return "?";
}

bool Search::EntryLess::operator()(const Entry &a, const Entry &b) const {
    if (a.priority != b.priority) {
        return a.priority < b.priority;
    }
    // Among equal priorities prefer the node closest to the goal, then the
    // oldest entry, so runs are deterministic.
    if (a.heuristic != b.heuristic) {
        return a.heuristic < b.heuristic;
    }
    return a.order < b.order;
}

Search::Search(const Grid &grid, Point start, Point goal, Algorithm algorithm)
    : grid_{grid}, start_{start}, goal_{goal}, algorithm_{algorithm}, cost_(grid.cell_count(), INT_MAX),
      parent_(grid.cell_count(), -1), state_(grid.cell_count(), CellState::Unseen) {
    if (!grid.is_open(start)) {
        throw std::invalid_argument("start " + start.str() + " is not an open cell");
    }
    if (!grid.is_open(goal)) {
        throw std::invalid_argument("end " + goal.str() + " is not an open cell");
    }
    add_to_frontier(grid.index(start), 0, -1);
}

void Search::add_to_frontier(int cell, int cost, int parent) {
    cost_[cell] = cost;
    parent_[cell] = parent;
    state_[cell] = CellState::Frontier;
    if (algorithm_ == Algorithm::BFS) {
        queue_.push_back(cell);
        return;
    }
    int heuristic = algorithm_ == Algorithm::AStar ? grid_.point(cell).manhattan_distance(goal_) : 0;
    heap_.push({cost + heuristic, heuristic, next_order_++, cell});
}

int Search::take_next() {
    if (algorithm_ == Algorithm::BFS) {
        if (queue_.empty()) {
            return -1;
        }
        int cell = queue_.front();
        queue_.pop_front();
        return cell;
    }
    // A cell can be pushed again when a cheaper route to it turns up; skip
    // the stale entries that were left behind.
    while (!heap_.empty()) {
        int cell = heap_.pop().cell;
        if (state_[cell] != CellState::Visited) {
            return cell;
        }
    }
    return -1;
}

void Search::step() {
    if (finished_) {
        return;
    }
    int cell = take_next();
    if (cell < 0) {
        finished_ = true;
        return;
    }
    state_[cell] = CellState::Visited;
    nodes_expanded_++;

    if (cell == grid_.index(goal_)) {
        for (int at = cell; at >= 0; at = parent_[at]) {
            path_.push_back(grid_.point(at));
            state_[at] = CellState::Path;
        }
        std::reverse(path_.begin(), path_.end());
        finished_ = true;
        return;
    }

    for (Point next : grid_.open_neighbours(grid_.point(cell))) {
        int index = grid_.index(next);
        if (state_[index] == CellState::Visited) {
            continue;
        }
        if (algorithm_ == Algorithm::BFS) {
            if (state_[index] == CellState::Unseen) {
                add_to_frontier(index, cost_[cell] + 1, cell);
            }
        } else if (cost_[cell] + 1 < cost_[index]) {
            add_to_frontier(index, cost_[cell] + 1, cell);
        }
    }
}

SearchResult solve(const Grid &grid, Point start, Point goal, Algorithm algorithm) {
    auto begin = std::chrono::steady_clock::now();
    Search search{grid, start, goal, algorithm};
    while (!search.finished()) {
        search.step();
    }
    auto end = std::chrono::steady_clock::now();
    return {algorithm, search.nodes_expanded(), search.path_length(), search.path(),
            std::chrono::duration<double, std::milli>(end - begin).count()};
}
