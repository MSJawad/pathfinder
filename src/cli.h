#ifndef PATHFINDER_CLI_H
#define PATHFINDER_CLI_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "grid.h"
#include "search.h"

struct Options {
    int size = 41;
    // Default to the top-left and bottom-right rooms of the maze.
    std::optional<Point> start;
    std::optional<Point> end;
    std::vector<Algorithm> algorithms{Algorithm::AStar, Algorithm::Dijkstra, Algorithm::BFS};
    // A random seed is picked (and printed) when none is given.
    std::optional<uint32_t> seed;
    int delay_ms = 5;
    bool headless = false;
    int runs = 1;
    std::string record_dir;
    bool help = false;
};

struct ParseResult {
    Options options;
    std::string error; // empty on success
};

// Parses the arguments after the program name.
ParseResult parse_args(const std::vector<std::string> &args);

std::string usage();

#endif
