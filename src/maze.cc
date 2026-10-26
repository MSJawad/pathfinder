#include "maze.h"

#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

constexpr Point kRoomSteps[] = {{0, -2}, {0, 2}, {-2, 0}, {2, 0}};

// std::mt19937 output is fully specified by the standard, but the standard
// distributions are not, so libc++ and libstdc++ would build different mazes
// from one seed. Reducing the raw output keeps seeds portable.
size_t random_index(std::mt19937 &rng, size_t count) { return rng() % count; }

bool is_room(const Grid &grid, Point p) {
    return p.x > 0 && p.y > 0 && p.x < grid.width() - 1 && p.y < grid.height() - 1 && p.x % 2 == 1 &&
           p.y % 2 == 1;
}

} // namespace

Grid generate_maze(int size, uint32_t seed) {
    if (size < 5 || size % 2 == 0) {
        throw std::invalid_argument("maze size must be an odd number of at least 5");
    }

    Grid grid{size, size};
    std::mt19937 rng{seed};
    const int rooms_per_side = (size - 1) / 2;

    std::vector<Point> frontier;
    std::vector<bool> in_frontier(grid.cell_count(), false);

    auto open_room = [&](Point room) {
        grid.set_wall(room, false);
        for (Point step : kRoomSteps) {
            Point next{room.x + step.x, room.y + step.y};
            if (is_room(grid, next) && grid.is_wall(next) && !in_frontier[grid.index(next)]) {
                in_frontier[grid.index(next)] = true;
                frontier.push_back(next);
            }
        }
    };

    Point first{1 + 2 * static_cast<int>(random_index(rng, rooms_per_side)),
                1 + 2 * static_cast<int>(random_index(rng, rooms_per_side))};
    open_room(first);

    while (!frontier.empty()) {
        size_t pick = random_index(rng, frontier.size());
        Point room = frontier[pick];
        frontier[pick] = frontier.back();
        frontier.pop_back();

        // Join the new room to a random room that is already part of the maze.
        std::vector<Point> joined;
        for (Point step : kRoomSteps) {
            Point next{room.x + step.x, room.y + step.y};
            if (is_room(grid, next) && !grid.is_wall(next)) {
                joined.push_back(next);
            }
        }
        Point target = joined[random_index(rng, joined.size())];
        grid.set_wall({(room.x + target.x) / 2, (room.y + target.y) / 2}, false);
        open_room(room);
    }

    return grid;
}
