#include "cli.h"

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <iterator>

namespace {

std::optional<long long> parse_integer(const std::string &text) {
    if (text.empty() || text.size() > 12) {
        return std::nullopt;
    }
    char *end = nullptr;
    long long value = std::strtoll(text.c_str(), &end, 10);
    if (*end != '\0') {
        return std::nullopt;
    }
    return value;
}

std::optional<Point> parse_point(const std::string &text) {
    size_t comma = text.find(',');
    if (comma == std::string::npos) {
        return std::nullopt;
    }
    auto x = parse_integer(text.substr(0, comma));
    auto y = parse_integer(text.substr(comma + 1));
    if (!x || !y || *x < INT_MIN || *x > INT_MAX || *y < INT_MIN || *y > INT_MAX) {
        return std::nullopt;
    }
    return Point{static_cast<int>(*x), static_cast<int>(*y)};
}

std::optional<std::vector<Algorithm>> parse_algorithms(const std::string &text) {
    if (text == "all") {
        return std::vector<Algorithm>{Algorithm::AStar, Algorithm::Dijkstra, Algorithm::BFS};
    }
    if (text == "astar" || text == "a*") {
        return std::vector<Algorithm>{Algorithm::AStar};
    }
    if (text == "dijkstra") {
        return std::vector<Algorithm>{Algorithm::Dijkstra};
    }
    if (text == "bfs") {
        return std::vector<Algorithm>{Algorithm::BFS};
    }
    return std::nullopt;
}

} // namespace

ParseResult parse_args(const std::vector<std::string> &args) {
    ParseResult result;
    Options &options = result.options;

    auto fail = [&](const std::string &message) {
        result.error = message;
        return result;
    };

    for (size_t i = 0; i < args.size(); i++) {
        std::string flag = args[i];
        std::optional<std::string> inline_value;
        size_t equals = flag.find('=');
        if (flag.rfind("--", 0) == 0 && equals != std::string::npos) {
            inline_value = flag.substr(equals + 1);
            flag = flag.substr(0, equals);
        }

        if (flag == "-h" || flag == "--help") {
            options.help = true;
            continue;
        }
        if (flag == "--headless") {
            options.headless = true;
            continue;
        }

        static const std::string value_flags[] = {"--size", "--start",    "--end",  "--algo",
                                                  "--seed", "--delay-ms", "--runs", "--record"};
        if (std::find(std::begin(value_flags), std::end(value_flags), flag) == std::end(value_flags)) {
            return fail("unknown option '" + args[i] + "'");
        }

        std::string value;
        if (inline_value) {
            value = *inline_value;
        } else if (i + 1 < args.size()) {
            value = args[++i];
        } else {
            return fail("missing value for " + flag);
        }

        if (flag == "--size") {
            auto size = parse_integer(value);
            if (!size || *size < 5 || *size > 1001 || *size % 2 == 0) {
                return fail("--size must be an odd number from 5 to 1001, got '" + value + "'");
            }
            options.size = static_cast<int>(*size);
        } else if (flag == "--start" || flag == "--end") {
            auto point = parse_point(value);
            if (!point) {
                return fail(flag + " must look like X,Y, got '" + value + "'");
            }
            (flag == "--start" ? options.start : options.end) = point;
        } else if (flag == "--algo") {
            auto algorithms = parse_algorithms(value);
            if (!algorithms) {
                return fail("--algo must be one of astar, dijkstra, bfs, all; got '" + value + "'");
            }
            options.algorithms = *algorithms;
        } else if (flag == "--seed") {
            auto seed = parse_integer(value);
            if (!seed || *seed < 0 || *seed > UINT32_MAX) {
                return fail("--seed must be a number from 0 to 4294967295, got '" + value + "'");
            }
            options.seed = static_cast<uint32_t>(*seed);
        } else if (flag == "--delay-ms") {
            auto delay = parse_integer(value);
            if (!delay || *delay < 0 || *delay > 10000) {
                return fail("--delay-ms must be a number from 0 to 10000, got '" + value + "'");
            }
            options.delay_ms = static_cast<int>(*delay);
        } else if (flag == "--runs") {
            auto runs = parse_integer(value);
            if (!runs || *runs < 1 || *runs > 100000) {
                return fail("--runs must be a number from 1 to 100000, got '" + value + "'");
            }
            options.runs = static_cast<int>(*runs);
        } else if (flag == "--record") {
            if (value.empty()) {
                return fail("--record needs a directory");
            }
            options.record_dir = value;
        }
    }

    if (options.runs > 1 && !options.headless) {
        return fail("--runs only works together with --headless");
    }
    if (options.headless && !options.record_dir.empty()) {
        return fail("--record and --headless cannot be combined");
    }
    return result;
}

std::string usage() {
    return "Usage: pathfinder [options]\n"
           "\n"
           "Generates a random maze and animates A*, Dijkstra and BFS solving it.\n"
           "\n"
           "Options:\n"
           "  --size N        maze width and height, odd, 5-1001 (default 41)\n"
           "  --start X,Y     start cell (default 1,1)\n"
           "  --end X,Y       end cell (default N-2,N-2)\n"
           "  --algo NAME     astar, dijkstra, bfs or all (default all)\n"
           "  --seed N        maze seed; the same seed gives the same maze (default random)\n"
           "  --delay-ms N    animation delay per expanded node (default 5)\n"
           "  --headless      print stats without opening a window\n"
           "  --runs N        with --headless, average over N seeds starting at --seed\n"
           "  --record DIR    render the animation offscreen to DIR as BMP frames\n"
           "  -h, --help      show this help\n"
           "\n"
           "Window keys: space pauses, R restarts, Esc or Q quits.\n";
}
