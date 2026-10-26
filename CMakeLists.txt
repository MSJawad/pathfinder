cmake_minimum_required(VERSION 3.16)
project(pathfinder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(PATHFINDER_BUILD_TESTS "Build the unit tests" ON)
option(PATHFINDER_SANITIZE "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(PATHFINDER_WERROR "Treat compiler warnings as errors" OFF)

if(PATHFINDER_SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
  add_link_options(-fsanitize=address,undefined)
endif()

function(pathfinder_warnings target)
  target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wno-sign-conversion)
  if(PATHFINDER_WERROR)
    target_compile_options(${target} PRIVATE -Werror)
  endif()
endfunction()

# Maze generation and search, with no graphics dependency.
add_library(pathfinder_core STATIC src/grid.cc src/maze.cc src/search.cc src/cli.cc)
target_include_directories(pathfinder_core PUBLIC src)
pathfinder_warnings(pathfinder_core)

find_package(SDL2 REQUIRED CONFIG)

add_executable(pathfinder src/main.cc src/renderer.cc src/font.cc)
target_link_libraries(pathfinder PRIVATE pathfinder_core SDL2::SDL2)
pathfinder_warnings(pathfinder)

if(PATHFINDER_BUILD_TESTS)
  enable_testing()

  include(FetchContent)
  if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
  endif()
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
    URL_HASH SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  add_executable(
    pathfinder_tests
    tests/cli_test.cc
    tests/grid_test.cc
    tests/maze_test.cc
    tests/min_heap_test.cc
    tests/search_test.cc)
  target_link_libraries(pathfinder_tests PRIVATE pathfinder_core GTest::gtest_main)
  pathfinder_warnings(pathfinder_tests)

  include(GoogleTest)
  gtest_discover_tests(pathfinder_tests)

  # End-to-end checks of the command-line program.
  add_test(NAME cli.headless_runs_all_algorithms COMMAND pathfinder --headless --seed 1)
  set_tests_properties(cli.headless_runs_all_algorithms PROPERTIES PASS_REGULAR_EXPRESSION "A\\*.*Dijkstra.*BFS")
  add_test(NAME cli.headless_exits_cleanly COMMAND pathfinder --headless --seed 7 --size 21 --runs 5)
  add_test(NAME cli.rejects_even_size COMMAND pathfinder --headless --size 40)
  set_tests_properties(cli.rejects_even_size PROPERTIES PASS_REGULAR_EXPRESSION "--size must be an odd number")
  add_test(NAME cli.rejects_start_on_wall COMMAND pathfinder --headless --seed 1 --start 0,0)
  set_tests_properties(cli.rejects_start_on_wall PROPERTIES PASS_REGULAR_EXPRESSION "start 0,0 is a wall")
  add_test(NAME cli.records_frames COMMAND pathfinder --seed 3 --size 11 --algo astar --record
                                           ${CMAKE_CURRENT_BINARY_DIR}/test-frames)
  set_tests_properties(cli.records_frames PROPERTIES PASS_REGULAR_EXPRESSION "Wrote [0-9]+ frames")
endif()
