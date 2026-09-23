#include "asciixel/core/image_sampler.hpp"

#include <cmath>
#include <stdexcept>

namespace asciixel {
ImagePixel ImageSampler::sampleBlock(const ImageFrame& frame,
                                     std::size_t       x_start,
                                     std::size_t       y_start,
                                     std::size_t       x_end,
                                     std::size_t       y_end)
{
    ImagePixel pixel{{0.0f, 0.0f, 0.0f}};
    for (std::size_t x = x_start; x < x_end; ++x) {
        for (std::size_t y = y_start; y < y_end; ++y) {
            pixel.color = pixel.color + frame.at(x, y).color;
        }
    }

    const float sample_count =
        static_cast<float>((x_end - x_start) * (y_end - y_start));
    pixel.color = {pixel.color.r / sample_count,
                   pixel.color.g / sample_count,
                   pixel.color.b / sample_count};
    return pixel;
}

ImageFrame ImageSampler::sample(const ImageFrame& frame,
                                std::size_t       new_width,
                                std::size_t       new_height)
{
    if (new_width == 0 || new_height == 0) {
        throw std::invalid_argument("sample dimensions must be greater than zero");
    }

    ImageFrame  sampled_frame(new_width, new_height);
    const float x_step = static_cast<float>(frame.width) / new_width;
    const float y_step = static_cast<float>(frame.height) / new_height;

    for (std::size_t y = 0; y < new_height; ++y) {
        for (std::size_t x = 0; x < new_width; ++x) {
            const std::size_t x_start = static_cast<std::size_t>(std::floor(x * x_step));
            const std::size_t y_start = static_cast<std::size_t>(std::floor(y * y_step));
            const std::size_t x_end   = static_cast<std::size_t>(std::ceil((x + 1) * x_step));
            const std::size_t y_end   = static_cast<std::size_t>(std::ceil((y + 1) * y_step));
            sampled_frame.at(x, y)    = sampleBlock(frame, x_start, y_start, x_end, y_end);
        }
    }

    return sampled_frame;
}

} // namespace asciixel