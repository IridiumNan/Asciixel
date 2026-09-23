#ifndef IMAGE_SAMPLER_HPP
#define IMAGE_SAMPLER_HPP

#include "asciixel/model/image_frame.hpp"
#include "asciixel/model/pixel.hpp"

namespace asciixel {

class ImageSampler {
private:
    static ImagePixel sampleBlock(const ImageFrame& frame,
                                  std::size_t       x_start,
                                  std::size_t       y_start,
                                  std::size_t       x_end,
                                  std::size_t       y_end);

public:
    static ImageFrame sample(const ImageFrame& frame,
                             std::size_t       new_width,
                             std::size_t       new_height);
};

} // namespace asciixel


#endif // IMAGE_SAMPLER_HPP