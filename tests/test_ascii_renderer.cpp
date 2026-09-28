#include "asciixel/core/ascii_renderer.hpp"
#include <limits>
#include <stdexcept>

void require(bool value) {
    if (!value) throw std::runtime_error("ASCII renderer assertion failed");
}
template<class F> void rejects(F action) {
    bool rejected = false;
    try { action(); } catch (const std::invalid_argument&) { rejected = true; }
    require(rejected);
}
int main() {
    asciixel::RasterizedCharset charset{{4, 5, 1, 2}, {
        {'j', 2, 3, -1, 1, {0, 128, 255, 64, 32, 255}, 0.0f},
        {' ', 0, 0, 0, 0, {}, 0.0f}
    }};
    asciixel::AsciiFrame frame(2, 2);
    frame.pixels[0].character = 'j';
    frame.pixels[1].character = ' ';
    frame.pixels[2].character = ' ';
    frame.pixels[3].character = 'j';
    const auto result = asciixel::renderAscii(frame, charset);
    require(result.width == 8 && result.height == 10);
    std::vector<std::uint8_t> expected(80, 0);
    expected[9] = 128; expected[16] = 255; expected[17] = 64;
    expected[24] = 32; expected[25] = 255;
    expected[53] = 128; expected[60] = 255; expected[61] = 64;
    expected[68] = 32; expected[69] = 255;
    require(result.pixels == expected);
    frame.pixels[0].character = '?';
    rejects([&] { asciixel::renderAscii(frame, charset); });
    frame.pixels[0].character = 'j';
    charset.glyphs[0].bitmap_left = -2;
    rejects([&] { asciixel::renderAscii(frame, charset); });
    charset.glyphs[0].bitmap_left = -1;
    charset.glyphs[0].alpha.pop_back();
    rejects([&] { asciixel::renderAscii(frame, charset); });
    charset.layout.cell_width = std::numeric_limits<std::size_t>::max();
    rejects([&] { asciixel::renderAscii(frame, charset); });
    rejects([&] { asciixel::renderAscii(asciixel::AsciiFrame(0, 0), charset); });
}
