#include "maze.h"

#include <gtest/gtest.h>

#include <queue>
#include <stdexcept>
#include <vector>

namespace {

int count_open(const Grid &grid) {
    int open = 0;
    for (int i = 0; i < grid.cell_count(); i++) {
        open += grid.is_wall(grid.point(i)) ? 0 : 1;
    }
    return open;
}

int count_reachable(const Grid &grid, Point from) {
    std::vector<bool> seen(grid.cell_count(), false);
    std::queue<Point> queue;
    queue.push(from);
    seen[grid.index(from)] = true;
    int reached = 0;
    while (!queue.empty()) {
        Point p = queue.front();
        queue.pop();
        reached++;
        for (Point next : grid.open_neighbours(p)) {
            if (!seen[grid.index(next)]) {
                seen[grid.index(next)] = true;
                queue.push(next);
            }
        }
    }
    return reached;
}

} // namespace

TEST(Maze, EveryOpenCellIsReachable) {
    for (int size : {5, 7, 21, 41, 101}) {
        for (uint32_t seed = 0; seed < 25; seed++) {
            Grid grid = generate_maze(size, seed);
            EXPECT_EQ(count_reachable(grid, {1, 1}), count_open(grid)) << "size " << size << " seed " << seed;
        }
    }
}

TEST(Maze, IsAPerfectMaze) {
    // Every room is open and the rooms are joined by exactly rooms - 1
    // passages, so the open cells form a tree with no loops.
    for (uint32_t seed = 0; seed < 25; seed++) {
        Grid grid = generate_maze(31, seed);
        int rooms = 0;
        for (int y = 1; y < grid.height(); y += 2) {
            for (int x = 1; x < grid.width(); x += 2) {
                EXPECT_FALSE(grid.is_wall({x, y}));
                rooms++;
            }
        }
        EXPECT_EQ(count_open(grid) - rooms, rooms - 1) << "seed " << seed;
    }
}

TEST(Maze, BorderAndPillarsAreWalls) {
    Grid grid = generate_maze(21, 9);
    for (int i = 0; i < 21; i++) {
        EXPECT_TRUE(grid.is_wall({i, 0}));
        EXPECT_TRUE(grid.is_wall({i, 20}));
        EXPECT_TRUE(grid.is_wall({0, i}));
        EXPECT_TRUE(grid.is_wall({20, i}));
    }
    for (int y = 0; y < 21; y += 2) {
        for (int x = 0; x < 21; x += 2) {
            EXPECT_TRUE(grid.is_wall({x, y}));
        }
    }
}

TEST(Maze, SameSeedGivesSameMaze) {
    Grid a = generate_maze(41, 1234);
    Grid b = generate_maze(41, 1234);
    Grid c = generate_maze(41, 1235);
    bool any_difference = false;
    for (int i = 0; i < a.cell_count(); i++) {
        ASSERT_EQ(a.is_wall(a.point(i)), b.is_wall(b.point(i)));
        any_difference |= a.is_wall(a.point(i)) != c.is_wall(c.point(i));
    }
    EXPECT_TRUE(any_difference);
}

TEST(Maze, RejectsBadSizes) {
    EXPECT_THROW(generate_maze(3, 0), std::invalid_argument);
    EXPECT_THROW(generate_maze(10, 0), std::invalid_argument);
}
