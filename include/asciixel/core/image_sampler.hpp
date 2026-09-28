#ifndef IMAGE_SAMPLER_HPP
#define IMAGE_SAMPLER_HPP

#include "asciixel/model/image_frame.hpp"
#include "asciixel/model/sampled_frame.hpp"

namespace asciixel {

class ImageSampler {
private:
    static SampledPixel sampleBlock(const ImageFrame& frame,
                                  double            x_start,
                                  double            y_start,
                                  double            x_end,
                                  double            y_end);

public:
    // The caller provides final grid dimensions, including character aspect correction.
    static SampledFrame sample(const ImageFrame& frame,
                               std::size_t       new_width,
                               std::size_t       new_height);
};

} // namespace asciixel


#endif // IMAGE_SAMPLER_HPP
