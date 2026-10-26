#include <filesystem>
#include <iomanip>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "cli.h"
#include "maze.h"
#include "renderer.h"
#include "search.h"

namespace {

struct Totals {
    Algorithm algorithm;
    double expanded = 0;
    double path = 0;
    double milliseconds = 0;
};

void print_table(const std::vector<Totals> &rows, int runs) {
    std::cout << std::left << std::setw(10) << "Algorithm" << std::right << std::setw(10) << "Expanded"
              << std::setw(8) << "Path" << std::setw(12) << "Time (ms)" << "\n";
    int precision = runs > 1 ? 1 : 0;
    for (const Totals &row : rows) {
        std::cout << std::left << std::setw(10) << algorithm_name(row.algorithm) << std::right << std::fixed
                  << std::setprecision(precision) << std::setw(10) << row.expanded / runs << std::setw(8)
                  << row.path / runs << std::setprecision(3) << std::setw(12) << row.milliseconds / runs << "\n";
    }
}

bool check_open(const Grid &grid, Point p, const char *name, uint32_t seed) {
    if (grid.is_open(p)) {
        return true;
    }
    std::cerr << "pathfinder: " << name << " " << p.str()
              << (grid.in_bounds(p) ? " is a wall" : " is outside the maze") << " (seed " << seed
              << "). Rooms at odd coordinates such as 1,1 are always open.\n";
    return false;
}

} // namespace

int main(int argc, char *argv[]) {
    ParseResult parsed = parse_args(std::vector<std::string>(argv + 1, argv + argc));
    if (!parsed.error.empty()) {
        std::cerr << "pathfinder: " << parsed.error << "\nRun 'pathfinder --help' for usage.\n";
        return 2;
    }
    const Options &options = parsed.options;
    if (options.help) {
        std::cout << usage();
        return 0;
    }

    const uint32_t seed = options.seed ? *options.seed : std::random_device{}();
    const Point start = options.start.value_or(Point{1, 1});
    const Point end = options.end.value_or(Point{options.size - 2, options.size - 2});

    std::vector<Totals> totals;
    for (Algorithm algorithm : options.algorithms) {
        totals.push_back({algorithm});
    }

    if (options.runs == 1) {
        std::cout << "Maze " << options.size << "x" << options.size << ", seed " << seed << ", start " << start.str()
                  << ", end " << end.str() << "\n";
    } else {
        std::cout << "Average over " << options.runs << " mazes of " << options.size << "x" << options.size
                  << " (seeds " << seed << " and up), start " << start.str() << ", end " << end.str() << "\n";
    }

    // The first maze is kept for the window, which shows the --seed maze.
    std::optional<Grid> first_grid;
    for (int run = 0; run < options.runs; run++) {
        const uint32_t run_seed = seed + static_cast<uint32_t>(run);
        Grid grid = generate_maze(options.size, run_seed);
        if (!check_open(grid, start, "start", run_seed) || !check_open(grid, end, "end", run_seed)) {
            return 2;
        }
        for (Totals &row : totals) {
            SearchResult result = solve(grid, start, end, row.algorithm);
            row.expanded += result.nodes_expanded;
            row.path += result.path_length;
            row.milliseconds += result.milliseconds;
        }
        if (run == 0) {
            first_grid = std::move(grid);
        }
    }
    print_table(totals, options.runs);

    if (options.headless) {
        return 0;
    }

    Visualizer visualizer{*first_grid, start, end, options.algorithms};
    if (!options.record_dir.empty()) {
        std::error_code error;
        std::filesystem::create_directories(options.record_dir, error);
        if (error) {
            std::cerr << "pathfinder: could not create " << options.record_dir << ": " << error.message() << "\n";
            return 1;
        }
        return visualizer.record(options.delay_ms, options.record_dir);
    }
    return visualizer.run_window(options.delay_ms, "Pathfinder - seed " + std::to_string(seed));
}
