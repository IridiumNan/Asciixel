#ifndef IMAGE_FRAME_HPP
#define IMAGE_FRAME_HPP

#include "asciixel/model/pixel.hpp"
#include <vector>

namespace asciixel {

struct ImageFrame {
    std::size_t              width;
    std::size_t              height;
    std::vector<ImagePixel> pixels;

    ImageFrame(std::size_t width, std::size_t height)
        : width(width), height(height)
    {
        pixels.resize(width * height);
    }

    ImagePixel& at(std::size_t x, std::size_t y)
    {
        return pixels[y * width + x];
    }
};

} // namespace asciixel

#endif