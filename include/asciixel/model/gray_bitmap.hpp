#ifndef ASCIIXEL_GRAY_BITMAP_HPP
#define ASCIIXEL_GRAY_BITMAP_HPP
#include <cstddef>
#include <cstdint>
#include <vector>

namespace asciixel {
// Opaque 8-bit grayscale, tightly packed, top row first (0 black, 255 white).
struct GrayBitmap {
    std::size_t               width  = 0;
    std::size_t               height = 0;
    std::vector<std::uint8_t> pixels;
};

// Bound the output buffer to 256 MiB before allocation.
inline constexpr std::size_t maxBitmapPixels = 256u * 1024u * 1024u;
} // namespace asciixel
#endif
