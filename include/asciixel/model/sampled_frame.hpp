#ifndef ASCIIXEL_SAMPLED_FRAME_HPP
#define ASCIIXEL_SAMPLED_FRAME_HPP

#include "asciixel/model/pixel.hpp"

#include <cstddef>
#include <vector>

namespace asciixel {

struct SampledFrame {
    std::size_t               width;
    std::size_t               height;
    std::vector<SampledPixel> pixels;

    SampledFrame(std::size_t width, std::size_t height)
        : width(width), height(height), pixels(width * height)
    {
    }

    SampledPixel& at(std::size_t x, std::size_t y)
    {
        return pixels[y * width + x];
    }

    const SampledPixel& at(std::size_t x, std::size_t y) const
    {
        return pixels[y * width + x];
    }
};

} // namespace asciixel

#endif // ASCIIXEL_SAMPLED_FRAME_HPP
