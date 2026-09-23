#ifndef ASCII_FRAME_HPP
#define ASCII_FRAME_HPP

#include "asciixel/model/pixel.hpp"
#include <vector>

namespace asciixel {

struct AsciiFrame {
    std::size_t              width;
    std::size_t              height;
    std::vector<AsciiPixel> pixels;

    AsciiFrame(std::size_t width, std::size_t height)
        : width(width), height(height)
    {
        pixels.resize(width * height);
    }

    AsciiPixel& at(std::size_t x, std::size_t y)
    {
        return pixels[y * width + x];
    }
};

} // namespace asciixel

#endif