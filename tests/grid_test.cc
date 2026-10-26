#include "grid.h"

#include <gtest/gtest.h>

// Regression test: the old Point copy constructor set y to the source's x.
TEST(Point, CopyKeepsBothCoordinates) {
    Point a{3, 7};
    Point b{a};
    EXPECT_EQ(b.x, 3);
    EXPECT_EQ(b.y, 7);
    EXPECT_EQ(b.str(), "3,7");
}

TEST(Point, ManhattanDistance) {
    EXPECT_EQ((Point{1, 1}.manhattan_distance({4, 5})), 7);
    EXPECT_EQ((Point{4, 5}.manhattan_distance({1, 1})), 7);
    EXPECT_EQ((Point{2, 2}.manhattan_distance({2, 2})), 0);
}

TEST(Grid, StartsAllWalls) {
    Grid grid{4, 3};
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 4; x++) {
            EXPECT_TRUE(grid.is_wall({x, y}));
        }
    }
}

TEST(Grid, IndexRoundTrips) {
    Grid grid{7, 5};
    for (int i = 0; i < grid.cell_count(); i++) {
        EXPECT_EQ(grid.index(grid.point(i)), i);
    }
}

TEST(Grid, OpenNeighboursSkipWallsAndEdges) {
    Grid grid{3, 3};
    grid.set_wall({0, 0}, false);
    grid.set_wall({1, 0}, false);
    grid.set_wall({0, 1}, false);
    EXPECT_EQ(grid.open_neighbours({0, 0}), (std::vector<Point>{{0, 1}, {1, 0}}));
    EXPECT_TRUE(grid.open_neighbours({2, 2}).empty());
    EXPECT_FALSE(grid.is_open({-1, 0}));
    EXPECT_FALSE(grid.is_open({3, 0}));
}

TEST(Grid, RejectsEmptyDimensions) {
    EXPECT_THROW((Grid{0, 3}), std::invalid_argument);
    EXPECT_THROW((Grid{3, -1}), std::invalid_argument);
}
