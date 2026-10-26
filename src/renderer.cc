// The original version of this project drew with the Xwindow helper class
// from the University of Waterloo's CS 246 course. The drawing now uses SDL2
// so it runs natively on macOS and Linux.

#include "renderer.h"

#include <SDL.h>

#include <algorithm>
#include <climits>
#include <cstdio>
#include <iostream>
#include <vector>

#include "font.h"

namespace {

struct Colour {
    uint8_t r, g, b;
};

constexpr Colour kBackground{0x12, 0x15, 0x1c};
constexpr Colour kText{0xe8, 0xeb, 0xf0};
constexpr Colour kSubtleText{0x9a, 0xa3, 0xb2};
constexpr Colour kFloor{0xf5, 0xf6, 0xf8};
constexpr Colour kGridLine{0xd5, 0xd9, 0xe0};
constexpr Colour kWall{0x2b, 0x32, 0x42};
constexpr Colour kFrontier{0xf5, 0xb8, 0x3d};
constexpr Colour kVisited{0x9c, 0xc9, 0xf5};
constexpr Colour kPath{0x8b, 0x5c, 0xf6};
constexpr Colour kStart{0x22, 0xc5, 0x5e};
constexpr Colour kEnd{0xef, 0x44, 0x44};

// Every colour a maze cell can take, so cells can be batched by colour.
enum CellColour { kWallCell, kFloorCell, kFrontierCell, kVisitedCell, kPathCell, kStartCell, kEndCell, kCellColours };

constexpr Colour kCellPalette[kCellColours] = {kWall, kFloor, kFrontier, kVisited, kPath, kStart, kEnd};

struct LegendItem {
    const char *label;
    Colour colour;
};

constexpr LegendItem kLegend[] = {
    {"Start", kStart}, {"End", kEnd},   {"Frontier", kFrontier},
    {"Visited", kVisited}, {"Path", kPath}, {"Wall", kWall},
};

constexpr int kTextScale = 2;
constexpr int kLineHeight = font::kGlyphHeight * kTextScale;
constexpr int kMargin = 16;
constexpr int kPanelGap = 32;
constexpr int kLegendSwatch = kLineHeight;
constexpr int kLegendRowHeight = kLineHeight + 10;
// Smaller cells would leave no room between the grid lines, so mazes that
// need them are drawn as one scaled texture per panel instead.
constexpr int kMinCell = 4;
constexpr int kMaxCell = 32;
constexpr int kFramesPerSecond = 30;

int text_width(const std::string &text, int scale = kTextScale) {
    if (text.empty()) {
        return 0;
    }
    return static_cast<int>(text.size()) * (font::kGlyphWidth + 1) * scale - scale;
}

// A panel's header is the algorithm name and status on one line with the
// stats below. The stats share a line when the panel is wide enough,
// otherwise they take two, and the text shrinks when even that would not fit.
struct HeaderStyle {
    int text_scale;
    int stats_lines;

    int line_pitch() const { return (font::kGlyphHeight + 4) * text_scale; }
    int height() const { return (1 + stats_lines) * line_pitch() - 4 * text_scale + 14; }
    // The widest stats the header is sized for; six digits covers --size 1001.
    int stats_width() const {
        return text_width(stats_lines == 1 ? "Expanded 000000  Path 000000" : "Expanded 000000", text_scale);
    }
};

constexpr HeaderStyle kHeaderStyles[] = {{kTextScale, 1}, {kTextScale, 2}, {1, 2}};

void set_colour(SDL_Renderer *renderer, Colour c) { SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 0xff); }

void fill(SDL_Renderer *renderer, int x, int y, int w, int h, Colour c) {
    set_colour(renderer, c);
    SDL_Rect rect{x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void draw_text(SDL_Renderer *renderer, int x, int y, const std::string &text, Colour c, int scale = kTextScale) {
    set_colour(renderer, c);
    for (char ch : text) {
        const uint8_t *rows = font::glyph(ch);
        for (int row = 0; row < font::kGlyphHeight; row++) {
            for (int col = 0; col < font::kGlyphWidth; col++) {
                if (rows[row] & (1 << (font::kGlyphWidth - 1 - col))) {
                    SDL_Rect pixel{x + col * scale, y + row * scale, scale, scale};
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        x += (font::kGlyphWidth + 1) * scale;
    }
}

// Lays the legend out in rows that fit width and calls place(item, x, row)
// for each entry. Returns the number of rows used.
template <typename Place> int layout_legend(int width, Place place) {
    int x = 0;
    int row = 0;
    for (const LegendItem &item : kLegend) {
        int item_width = kLegendSwatch + 6 + text_width(item.label);
        if (x > 0 && x + item_width > width) {
            x = 0;
            row++;
        }
        place(item, x, row);
        x += item_width + 18;
    }
    return row + 1;
}

CellColour cell_colour(const Grid &grid, const Search &search, Point p, Point start, Point end) {
    if (grid.is_wall(p)) {
        return kWallCell;
    }
    if (p == start) {
        return kStartCell;
    }
    if (p == end) {
        return kEndCell;
    }
    switch (search.state(p)) {
    case CellState::Unseen:
        return kFloorCell;
    case CellState::Frontier:
        return kFrontierCell;
    case CellState::Visited:
        return kVisitedCell;
    case CellState::Path:
        return kPathCell;
    }
    return kFloorCell;
}

} // namespace

Visualizer::Visualizer(const Grid &grid, Point start, Point end, std::vector<Algorithm> algorithms)
    : grid_{grid}, start_{start}, end_{end}, algorithms_{std::move(algorithms)} {
    restart();
}

void Visualizer::restart() {
    searches_.clear();
    for (Algorithm algorithm : algorithms_) {
        searches_.emplace_back(grid_, start_, end_, algorithm);
    }
}

bool Visualizer::all_finished() const {
    return std::all_of(searches_.begin(), searches_.end(), [](const Search &s) { return s.finished(); });
}

void Visualizer::advance(int steps) {
    for (Search &search : searches_) {
        for (int i = 0; i < steps && !search.finished(); i++) {
            search.step();
        }
    }
}

Visualizer::Layout Visualizer::layout_for(int max_width, int max_height) const {
    const int panels = static_cast<int>(searches_.size());
    const int size = std::max(grid_.width(), grid_.height());
    const int panel_room = (max_width - 2 * kMargin - (panels - 1) * kPanelGap) / panels;

    // The roomiest header whose stats fit the panel, or else the tightest one.
    HeaderStyle header = kHeaderStyles[0];
    for (HeaderStyle candidate : kHeaderStyles) {
        header = candidate;
        if (header.stats_width() <= panel_room) {
            break;
        }
    }

    Layout layout;
    layout.margin = kMargin;
    layout.text_scale = header.text_scale;
    layout.stats_lines = header.stats_lines;
    layout.header_height = header.height();

    int height_room = max_height - 2 * kMargin - layout.header_height - kPanelGap - 2 * kLegendRowHeight;
    int fit = std::min((panel_room - 1) / size, (height_room - 1) / size);
    if (fit >= kMinCell) {
        layout.cell = std::min(fit, kMaxCell);
        layout.maze_pixels = size * layout.cell + 1;
    } else {
        layout.cell = 0;
        layout.maze_pixels = std::max(64, std::min(panel_room, height_room));
    }

    layout.panel_width = std::max(layout.maze_pixels, std::min(header.stats_width(), panel_room));
    layout.width = 2 * kMargin + panels * layout.panel_width + (panels - 1) * kPanelGap;
    layout.legend_top = kMargin + layout.header_height + layout.maze_pixels + kPanelGap;
    int legend_rows = layout_legend(layout.width - 2 * kMargin, [](const LegendItem &, int, int) {});
    layout.height = layout.legend_top + legend_rows * kLegendRowHeight - (kLegendRowHeight - kLineHeight) + kMargin;
    return layout;
}

void Visualizer::draw(SDL_Renderer *renderer, const Layout &layout) {
    set_colour(renderer, kBackground);
    SDL_RenderClear(renderer);

    const HeaderStyle header{layout.text_scale, layout.stats_lines};
    for (size_t i = 0; i < searches_.size(); i++) {
        const Search &search = searches_[i];
        int panel_x = layout.margin + static_cast<int>(i) * (layout.panel_width + kPanelGap);
        int top = layout.margin;
        int scale = header.text_scale;

        draw_text(renderer, panel_x, top, algorithm_name(search.algorithm()), kText, scale);
        std::string status = paused_ ? "Paused" : !search.finished() ? "" : search.found() ? "Done" : "No path";
        draw_text(renderer, panel_x + layout.panel_width - text_width(status, scale), top, status, kSubtleText, scale);
        std::string expanded = "Expanded " + std::to_string(search.nodes_expanded());
        std::string path = "Path " + (search.found() ? std::to_string(search.path_length()) : "-");
        if (header.stats_lines == 1) {
            draw_text(renderer, panel_x, top + header.line_pitch(), expanded + "  " + path, kSubtleText, scale);
        } else {
            draw_text(renderer, panel_x, top + header.line_pitch(), expanded, kSubtleText, scale);
            draw_text(renderer, panel_x, top + 2 * header.line_pitch(), path, kSubtleText, scale);
        }

        int maze_x = panel_x + (layout.panel_width - layout.maze_pixels) / 2;
        int maze_y = top + layout.header_height;
        if (layout.cell > 0) {
            draw_cells(renderer, search, maze_x, maze_y, layout.cell);
        } else {
            draw_texture(renderer, i, maze_x, maze_y, layout.maze_pixels);
        }
    }

    layout_legend(layout.width - 2 * layout.margin, [&](const LegendItem &item, int x, int row) {
        int left = layout.margin + x;
        int top = layout.legend_top + row * kLegendRowHeight;
        fill(renderer, left, top, kLegendSwatch, kLegendSwatch, item.colour);
        draw_text(renderer, left + kLegendSwatch + 6, top, item.label, kSubtleText);
    });
}

void Visualizer::draw_cells(SDL_Renderer *renderer, const Search &search, int x, int y, int cell) const {
    // Open cells are inset by one pixel over the grid-line colour, which
    // leaves crisp lines between them. Walls cover their own line pixels so
    // neighbouring walls merge into solid blocks. Cells are batched by colour
    // so a frame costs one draw call per colour.
    int pixels = grid_.width() * cell + 1;
    fill(renderer, x, y, pixels, grid_.height() * cell + 1, kGridLine);

    std::vector<SDL_Rect> batches[kCellColours];
    for (int row = 0; row < grid_.height(); row++) {
        for (int col = 0; col < grid_.width(); col++) {
            CellColour colour = cell_colour(grid_, search, {col, row}, start_, end_);
            int px = x + col * cell;
            int py = y + row * cell;
            if (colour == kWallCell) {
                batches[colour].push_back({px, py, cell + 1, cell + 1});
            } else {
                batches[colour].push_back({px + 1, py + 1, cell - 1, cell - 1});
            }
        }
    }
    for (int colour = 0; colour < kCellColours; colour++) {
        if (!batches[colour].empty()) {
            set_colour(renderer, kCellPalette[colour]);
            SDL_RenderFillRects(renderer, batches[colour].data(), static_cast<int>(batches[colour].size()));
        }
    }
}

void Visualizer::draw_texture(SDL_Renderer *renderer, size_t panel, int x, int y, int pixels) {
    // One texel per cell, scaled to fit the panel, keeps very large mazes
    // inside the window and cheap to draw. Shrinking blends neighbouring
    // cells instead of dropping whole rows of them.
    if (textures_.size() != searches_.size()) {
        release_textures();
        for (size_t i = 0; i < searches_.size(); i++) {
            PanelTexture panel_texture;
            panel_texture.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                                      SDL_TEXTUREACCESS_STREAMING, grid_.width(), grid_.height());
            if (panel_texture.texture) {
                SDL_SetTextureScaleMode(panel_texture.texture,
                                        pixels < grid_.width() ? SDL_ScaleModeLinear : SDL_ScaleModeNearest);
            }
            panel_texture.pixels.assign(grid_.cell_count(), 0);
            // No real state matches this, so the first frame paints every cell.
            panel_texture.drawn.assign(grid_.cell_count(), static_cast<CellState>(0xff));
            textures_.push_back(std::move(panel_texture));
        }
    }

    PanelTexture &panel_texture = textures_[panel];
    if (!panel_texture.texture) {
        fill(renderer, x, y, pixels, pixels, kWall);
        return;
    }
    const Search &search = searches_[panel];
    const std::vector<CellState> &states = search.states();
    for (int i = 0; i < grid_.cell_count(); i++) {
        if (states[i] == panel_texture.drawn[i]) {
            continue;
        }
        panel_texture.drawn[i] = states[i];
        Colour c = kCellPalette[cell_colour(grid_, search, grid_.point(i), start_, end_)];
        panel_texture.pixels[i] =
            0xff000000u | static_cast<uint32_t>(c.r) << 16 | static_cast<uint32_t>(c.g) << 8 | c.b;
    }
    SDL_UpdateTexture(panel_texture.texture, nullptr, panel_texture.pixels.data(),
                      grid_.width() * static_cast<int>(sizeof(uint32_t)));
    SDL_Rect target{x, y, pixels, pixels};
    SDL_RenderCopy(renderer, panel_texture.texture, nullptr, &target);
}

void Visualizer::release_textures() {
    for (PanelTexture &panel_texture : textures_) {
        if (panel_texture.texture) {
            SDL_DestroyTexture(panel_texture.texture);
        }
    }
    textures_.clear();
}

int Visualizer::run_window(int delay_ms, const std::string &title) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "error: could not start SDL: " << SDL_GetError() << "\n"
                  << "Use --headless to print the stats without a window.\n";
        return 1;
    }

    SDL_Rect usable{0, 0, 1280, 800};
    if (SDL_GetDisplayUsableBounds(0, &usable) != 0) {
        usable = {0, 0, 1280, 800};
    }
    Layout layout = layout_for(usable.w * 9 / 10, usable.h * 9 / 10);

    SDL_Window *window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          layout.width, layout.height, SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        std::cerr << "error: could not open a window: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!renderer) {
        std::cerr << "error: could not create a renderer: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // On high-DPI screens the drawable is larger than the window; scale the
    // layout up so lines stay sharp instead of being blurred by the compositor.
    int drawable_width = 0;
    int drawable_height = 0;
    SDL_GetRendererOutputSize(renderer, &drawable_width, &drawable_height);
    float scale = std::max(1.0f, static_cast<float>(drawable_width) / static_cast<float>(layout.width));
    SDL_RenderSetScale(renderer, scale, scale);

    const int frame_ms = 1000 / 60;
    uint64_t last_tick = SDL_GetTicks64();
    int pending_ms = 0;
    bool running = true;
    while (running) {
        SDL_Event event;
        bool idle = paused_ || all_finished();
        bool got_event = idle ? SDL_WaitEventTimeout(&event, 250) : SDL_PollEvent(&event);
        while (got_event) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                case SDLK_q:
                    running = false;
                    break;
                case SDLK_SPACE:
                    paused_ = !paused_;
                    break;
                case SDLK_r:
                    restart();
                    break;
                default:
                    break;
                }
            }
            got_event = SDL_PollEvent(&event);
        }

        uint64_t now = SDL_GetTicks64();
        int elapsed = static_cast<int>(std::min<uint64_t>(now - last_tick, 250));
        last_tick = now;
        if (paused_ || all_finished()) {
            pending_ms = 0;
        } else if (delay_ms == 0) {
            advance(INT_MAX);
        } else {
            pending_ms += elapsed;
            advance(pending_ms / delay_ms);
            pending_ms %= delay_ms;
        }

        draw(renderer, layout);
        SDL_RenderPresent(renderer);

        int spent = static_cast<int>(SDL_GetTicks64() - now);
        if (spent < frame_ms) {
            SDL_Delay(frame_ms - spent);
        }
    }

    release_textures();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int Visualizer::record(int delay_ms, const std::string &directory) {
    Layout layout = layout_for(1280, 800);
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, layout.width, layout.height, 24, SDL_PIXELFORMAT_RGB24);
    SDL_Renderer *renderer = surface ? SDL_CreateSoftwareRenderer(surface) : nullptr;
    if (!renderer) {
        std::cerr << "error: could not create an offscreen renderer: " << SDL_GetError() << "\n";
        SDL_FreeSurface(surface);
        return 1;
    }

    int frames = 0;
    auto save_frame = [&]() {
        draw(renderer, layout);
        SDL_RenderPresent(renderer);
        char name[32];
        std::snprintf(name, sizeof name, "/frame-%05d.bmp", frames++);
        return SDL_SaveBMP(surface, (directory + name).c_str()) == 0;
    };

    bool ok = save_frame();
    const int frame_ms = 1000 / kFramesPerSecond;
    int pending_ms = 0;
    while (ok && !all_finished()) {
        if (delay_ms == 0) {
            advance(INT_MAX);
        } else {
            pending_ms += frame_ms;
            advance(pending_ms / delay_ms);
            pending_ms %= delay_ms;
        }
        ok = save_frame();
    }

    release_textures();
    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(surface);
    if (!ok) {
        std::cerr << "error: could not write frames to " << directory << ": " << SDL_GetError() << "\n";
        return 1;
    }
    std::cout << "Wrote " << frames << " frames (" << layout.width << "x" << layout.height << ", "
              << kFramesPerSecond << " fps) to " << directory << "\n";
    return 0;
}
