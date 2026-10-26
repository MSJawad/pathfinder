#ifndef PATHFINDER_RENDERER_H
#define PATHFINDER_RENDERER_H

#include <cstdint>
#include <string>
#include <vector>

#include "grid.h"
#include "search.h"

struct SDL_Renderer;

// Draws one maze panel per algorithm side by side and steps every search
// together, so their frontiers can be compared on the same maze.
class Visualizer {
  public:
    struct Layout {
        int cell = 0;
        int margin = 0;
        int panel_width = 0;
        int header_height = 0;
        int maze_pixels = 0;
        int legend_top = 0;
        int width = 0;
        int height = 0;
    };

    Visualizer(const Grid &grid, Point start, Point end, std::vector<Algorithm> algorithms);

    // Opens a window and animates until the user closes it. Returns an exit code.
    int run_window(int delay_ms, const std::string &title);

    // Renders the animation offscreen and writes one BMP per frame into
    // directory, which must exist. Returns an exit code.
    int record(int delay_ms, const std::string &directory);

  private:
    const Grid &grid_;
    Point start_;
    Point end_;
    std::vector<Algorithm> algorithms_;
    std::vector<Search> searches_;
    bool paused_ = false;

    void restart();
    bool all_finished() const;
    void advance(int steps);
    Layout layout_for(int max_width, int max_height) const;
    void draw(SDL_Renderer *renderer, const Layout &layout) const;
};

#endif
