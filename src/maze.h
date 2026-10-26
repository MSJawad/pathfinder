#ifndef PATHFINDER_MAZE_H
#define PATHFINDER_MAZE_H

#include <cstdint>

#include "grid.h"

// Generates a size x size maze with randomized Prim's algorithm. Rooms sit on
// odd coordinates and the outer border is all wall, so size must be odd and at
// least 5. The result is a perfect maze: every open cell is reachable and
// there is exactly one path between any two rooms.
//
// The same seed gives the same maze on every platform.
Grid generate_maze(int size, uint32_t seed);

#endif
