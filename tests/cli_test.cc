#include "cli.h"

#include <gtest/gtest.h>

namespace {

ParseResult parse(std::vector<std::string> args) { return parse_args(args); }

} // namespace

// Regression test: the old program read coordinates from stdin and crashed on
// empty input. Running with no arguments now uses defaults.
TEST(Cli, DefaultsWithNoArguments) {
    ParseResult result = parse({});
    ASSERT_EQ(result.error, "");
    const Options &options = result.options;
    EXPECT_EQ(options.size, 41);
    EXPECT_FALSE(options.start.has_value());
    EXPECT_FALSE(options.end.has_value());
    EXPECT_EQ(options.algorithms.size(), 3u);
    EXPECT_FALSE(options.seed.has_value());
    EXPECT_EQ(options.delay_ms, 5);
    EXPECT_FALSE(options.headless);
    EXPECT_EQ(options.runs, 1);
}

TEST(Cli, ParsesEveryOption) {
    ParseResult result = parse({"--size", "21", "--start", "1,3", "--end", "19,17", "--algo", "dijkstra", "--seed",
                                "4294967295", "--delay-ms", "0", "--headless", "--runs", "10"});
    ASSERT_EQ(result.error, "");
    const Options &options = result.options;
    EXPECT_EQ(options.size, 21);
    EXPECT_EQ(*options.start, (Point{1, 3}));
    EXPECT_EQ(*options.end, (Point{19, 17}));
    EXPECT_EQ(options.algorithms, std::vector<Algorithm>{Algorithm::Dijkstra});
    EXPECT_EQ(*options.seed, 4294967295u);
    EXPECT_EQ(options.delay_ms, 0);
    EXPECT_TRUE(options.headless);
    EXPECT_EQ(options.runs, 10);
}

TEST(Cli, ParsesAlgorithmNames) {
    EXPECT_EQ(parse({"--algo", "astar"}).options.algorithms, std::vector<Algorithm>{Algorithm::AStar});
    EXPECT_EQ(parse({"--algo", "bfs"}).options.algorithms, std::vector<Algorithm>{Algorithm::BFS});
    EXPECT_EQ(parse({"--algo", "all"}).options.algorithms.size(), 3u);
}

TEST(Cli, Help) {
    EXPECT_TRUE(parse({"--help"}).options.help);
    EXPECT_TRUE(parse({"-h"}).options.help);
    EXPECT_NE(usage().find("--delay-ms"), std::string::npos);
}

TEST(Cli, RejectsBadInput) {
    const std::vector<std::vector<std::string>> bad{
        {"--size"},
        {"--size", "40"},
        {"--size", "3"},
        {"--size", "abc"},
        {"--size", "21x"},
        {"--start", "1"},
        {"--start", "1,"},
        {"--start", "a,b"},
        {"--algo", "dfs"},
        {"--seed", "-1"},
        {"--seed", "4294967296"},
        {"--delay-ms", "-5"},
        {"--runs", "0"},
        {"--runs", "5"},
        {"--record", "frames", "--headless"},
        {"--bogus"},
        {"--bogus=1"},
        {"--size=21"},
        {"--algo", "a*"},
        {"extra"},
    };
    for (const auto &args : bad) {
        std::string joined;
        for (const auto &arg : args) {
            joined += arg + " ";
        }
        EXPECT_NE(parse(args).error, "") << joined;
    }
}

TEST(Cli, UnknownOptionIsNamed) {
    EXPECT_EQ(parse({"--bogus"}).error, "unknown option '--bogus'");
    EXPECT_EQ(parse({"--bogus=1"}).error, "unknown option '--bogus=1'");
}

// Only the documented "--flag value" spelling parses, so a value glued on
// with '=' is never silently dropped.
TEST(Cli, RejectsInlineValues) {
    EXPECT_EQ(parse({"--headless=no"}).error, "unknown option '--headless=no'");
    EXPECT_EQ(parse({"--help=0"}).error, "unknown option '--help=0'");
    EXPECT_EQ(parse({"--seed=5"}).error, "unknown option '--seed=5'");
}
