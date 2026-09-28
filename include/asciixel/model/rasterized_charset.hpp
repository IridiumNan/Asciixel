#ifndef ASCIIXEL_RASTERIZED_CHARSET_HPP
#define ASCIIXEL_RASTERIZED_CHARSET_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

namespace asciixel {

struct GlyphLayout {
    std::size_t cell_width = 0;
    std::size_t cell_height = 0;
    // Baseline origin relative to the cell's top-left corner; y increases downwards.
    int baseline_x = 0;
    int baseline_y = 0;
};

struct RasterizedGlyph {
    char character = ' ';
    std::size_t bitmap_width = 0;
    std::size_t bitmap_height = 0;
    int bitmap_left = 0;
    int bitmap_top = 0;
    // Owned, tightly packed grayscale coverage, top row first.
    std::vector<std::uint8_t> alpha;
    float density = 0; // Raw coverage divided by the common cell area.
};

struct RasterizedCharset {
    GlyphLayout layout;
    std::vector<RasterizedGlyph> glyphs;
};

} // namespace asciixel
#endif
