#include "grid.h"

#include <stdexcept>

std::string Point::str() const { return std::to_string(x) + ',' + std::to_string(y); }

namespace {

size_t checked_area(int width, int height) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("grid dimensions must be positive");
    }
    return static_cast<size_t>(width) * static_cast<size_t>(height);
}

} // namespace

Grid::Grid(int width, int height) : width_{width}, height_{height}, walls_(checked_area(width, height), true) {}

std::vector<Point> Grid::open_neighbours(Point p) const {
    static constexpr Point directions[] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    std::vector<Point> result;
    for (Point d : directions) {
        Point next{p.x + d.x, p.y + d.y};
        if (is_open(next)) {
            result.push_back(next);
        }
    }
    return result;
}
