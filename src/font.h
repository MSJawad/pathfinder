#ifndef PATHFINDER_FONT_H
#define PATHFINDER_FONT_H

#include <cstdint>

// A tiny built-in 5x7 bitmap font, so labels need no font files or extra
// libraries. Covers A-Z (lowercase is drawn as uppercase), 0-9 and * : . , - ( ) /.
namespace font {

constexpr int kGlyphWidth = 5;
constexpr int kGlyphHeight = 7;

// Returns kGlyphHeight rows; bit 4 of each row is the leftmost pixel.
// Unknown characters come back blank.
const uint8_t *glyph(char c);

} // namespace font

#endif
