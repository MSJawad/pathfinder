#ifndef PATHFINDER_RENDERER_H
#define PATHFINDER_RENDERER_H

#include <cstdint>
#include <string>
#include <vector>

#include "grid.h"
#include "search.h"

struct SDL_Renderer;
struct SDL_Texture;

// Draws one maze panel per algorithm side by side and steps every search
// together, so their frontiers can be compared on the same maze.
class Visualizer {
  public:
    struct Layout {
        // Pixels per maze cell, or 0 when the maze is too big for whole
        // cells and is drawn as a scaled texture instead.
        int cell = 0;
        int margin = 0;
        int panel_width = 0;
        // Header text size, and whether the stats take one line or two.
        int text_scale = 0;
        int stats_lines = 0;
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
    // Used when the maze is too big to draw cell by cell: a one-texel-per-cell
    // texture per panel, plus the cell states it currently shows so each
    // frame only repaints the cells that changed.
    struct PanelTexture {
        SDL_Texture *texture = nullptr;
        std::vector<uint32_t> pixels;
        std::vector<CellState> drawn;
    };
    std::vector<PanelTexture> textures_;
    bool paused_ = false;

    void restart();
    bool all_finished() const;
    void advance(int steps);
    Layout layout_for(int max_width, int max_height) const;
    void draw(SDL_Renderer *renderer, const Layout &layout);
    void draw_cells(SDL_Renderer *renderer, const Search &search, int x, int y, int cell) const;
    void draw_texture(SDL_Renderer *renderer, size_t panel, int x, int y, int pixels);
    void release_textures();
};

#endif
