#ifndef PATHFINDER_GRID_H
#define PATHFINDER_GRID_H

#include <cstdlib>
#include <string>
#include <vector>

struct Point {
    int x = 0;
    int y = 0;

    bool operator==(const Point &other) const { return x == other.x && y == other.y; }
    bool operator!=(const Point &other) const { return !(*this == other); }

    int manhattan_distance(const Point &other) const {
        return std::abs(x - other.x) + std::abs(y - other.y);
    }

    std::string str() const;
};

// A rectangular grid of open cells and walls. Movement is 4-directional and
// every step costs 1.
class Grid {
    int width_;
    int height_;
    std::vector<bool> walls_;

  public:
    // Starts with every cell set to a wall.
    Grid(int width, int height);

    int width() const { return width_; }
    int height() const { return height_; }
    int cell_count() const { return width_ * height_; }

    bool in_bounds(Point p) const { return p.x >= 0 && p.x < width_ && p.y >= 0 && p.y < height_; }
    bool is_wall(Point p) const { return walls_[index(p)]; }
    bool is_open(Point p) const { return in_bounds(p) && !is_wall(p); }
    void set_wall(Point p, bool wall) { walls_[index(p)] = wall; }

    int index(Point p) const { return p.y * width_ + p.x; }
    Point point(int index) const { return {index % width_, index / width_}; }

    // Open in-bounds neighbours of p, in the fixed order up, down, left, right.
    std::vector<Point> open_neighbours(Point p) const;
};

#endif
