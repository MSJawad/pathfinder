#include "search.h"

#include <gtest/gtest.h>

#include <queue>
#include <random>
#include <stdexcept>
#include <vector>

#include "maze.h"

namespace {

constexpr Algorithm kAlgorithms[] = {Algorithm::AStar, Algorithm::Dijkstra, Algorithm::BFS};

// An open grid with randomly placed walls. Unlike a perfect maze it has many
// routes between two cells, so a search that is not optimal shows up.
Grid random_grid(int width, int height, double wall_chance, std::mt19937 &rng) {
    Grid grid{width, height};
    std::bernoulli_distribution wall{wall_chance};
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            grid.set_wall({x, y}, wall(rng));
        }
    }
    return grid;
}

Point random_open_cell(const Grid &grid, std::mt19937 &rng) {
    while (true) {
        Point p{static_cast<int>(rng() % grid.width()), static_cast<int>(rng() % grid.height())};
        if (!grid.is_wall(p)) {
            return p;
        }
    }
}

// Shortest distance computed independently of Search, or -1 if unreachable.
int reference_distance(const Grid &grid, Point from, Point to) {
    std::vector<int> distance(grid.cell_count(), -1);
    std::queue<Point> queue;
    distance[grid.index(from)] = 0;
    queue.push(from);
    while (!queue.empty()) {
        Point p = queue.front();
        queue.pop();
        for (Point next : grid.open_neighbours(p)) {
            if (distance[grid.index(next)] < 0) {
                distance[grid.index(next)] = distance[grid.index(p)] + 1;
                queue.push(next);
            }
        }
    }
    return distance[grid.index(to)];
}

void expect_valid_path(const Grid &grid, const std::vector<Point> &path, Point start, Point goal) {
    ASSERT_FALSE(path.empty());
    EXPECT_EQ(path.front(), start);
    EXPECT_EQ(path.back(), goal);
    for (size_t i = 0; i < path.size(); i++) {
        EXPECT_TRUE(grid.is_open(path[i]));
        if (i > 0) {
            EXPECT_EQ(path[i].manhattan_distance(path[i - 1]), 1);
        }
    }
}

} // namespace

TEST(Search, AllAlgorithmsFindShortestPathsOnRandomGrids) {
    std::mt19937 rng{42};
    int reachable = 0;
    for (int trial = 0; trial < 500; trial++) {
        Grid grid = random_grid(30, 20, 0.3, rng);
        Point start = random_open_cell(grid, rng);
        Point goal = random_open_cell(grid, rng);
        int expected = reference_distance(grid, start, goal);
        reachable += expected >= 0 ? 1 : 0;

        for (Algorithm algorithm : kAlgorithms) {
            SearchResult result = solve(grid, start, goal, algorithm);
            ASSERT_EQ(result.path_length, expected)
                << algorithm_name(algorithm) << " trial " << trial << " from " << start.str() << " to " << goal.str();
            if (expected >= 0) {
                expect_valid_path(grid, result.path, start, goal);
            } else {
                EXPECT_TRUE(result.path.empty());
            }
        }
    }
    // Make sure the trials exercised both outcomes.
    EXPECT_GT(reachable, 250);
    EXPECT_LT(reachable, 500);
}

TEST(Search, AStarMatchesBfsOnMazes) {
    for (uint32_t seed = 0; seed < 50; seed++) {
        Grid grid = generate_maze(41, seed);
        Point start{1, 1};
        Point goal{39, 39};
        SearchResult astar = solve(grid, start, goal, Algorithm::AStar);
        SearchResult bfs = solve(grid, start, goal, Algorithm::BFS);
        SearchResult dijkstra = solve(grid, start, goal, Algorithm::Dijkstra);
        EXPECT_EQ(astar.path_length, bfs.path_length) << "seed " << seed;
        EXPECT_EQ(dijkstra.path_length, bfs.path_length) << "seed " << seed;
        EXPECT_EQ(astar.path, bfs.path) << "a perfect maze has exactly one path, seed " << seed;
        EXPECT_LE(astar.nodes_expanded, bfs.nodes_expanded) << "seed " << seed;
    }
}

TEST(Search, AStarExpandsFewerNodesOnOpenGrid) {
    Grid grid{41, 41};
    for (int i = 0; i < grid.cell_count(); i++) {
        grid.set_wall(grid.point(i), false);
    }
    SearchResult astar = solve(grid, {0, 0}, {40, 40}, Algorithm::AStar);
    SearchResult bfs = solve(grid, {0, 0}, {40, 40}, Algorithm::BFS);
    EXPECT_EQ(astar.path_length, 80);
    EXPECT_EQ(bfs.path_length, 80);
    // The tie-break on distance to the goal walks straight there.
    EXPECT_EQ(astar.nodes_expanded, 81);
    EXPECT_EQ(bfs.nodes_expanded, grid.cell_count());
}

TEST(Search, StartEqualsGoal) {
    Grid grid = generate_maze(11, 0);
    for (Algorithm algorithm : kAlgorithms) {
        SearchResult result = solve(grid, {5, 5}, {5, 5}, algorithm);
        EXPECT_EQ(result.path_length, 0);
        EXPECT_EQ(result.nodes_expanded, 1);
        EXPECT_EQ(result.path, (std::vector<Point>{{5, 5}}));
    }
}

TEST(Search, RejectsWallsAndOutOfBoundsEndpoints) {
    Grid grid = generate_maze(11, 0);
    EXPECT_THROW((Search{grid, {0, 0}, {1, 1}, Algorithm::AStar}), std::invalid_argument);
    EXPECT_THROW((Search{grid, {1, 1}, {0, 0}, Algorithm::BFS}), std::invalid_argument);
    EXPECT_THROW((Search{grid, {1, 1}, {11, 1}, Algorithm::Dijkstra}), std::invalid_argument);
    EXPECT_THROW((Search{grid, {-1, 1}, {1, 1}, Algorithm::Dijkstra}), std::invalid_argument);
}

TEST(Search, StepsOneNodeAtATime) {
    Grid grid = generate_maze(21, 5);
    Search search{grid, {1, 1}, {19, 19}, Algorithm::AStar};
    EXPECT_EQ(search.state({1, 1}), CellState::Frontier);
    EXPECT_FALSE(search.found());
    EXPECT_EQ(search.path_length(), -1);

    int steps = 0;
    while (!search.finished()) {
        search.step();
        steps++;
        EXPECT_EQ(search.nodes_expanded(), steps);
    }
    EXPECT_TRUE(search.found());
    EXPECT_EQ(search.state({1, 1}), CellState::Path);
    EXPECT_EQ(search.state({19, 19}), CellState::Path);

    search.step();
    EXPECT_EQ(search.nodes_expanded(), steps);
}
