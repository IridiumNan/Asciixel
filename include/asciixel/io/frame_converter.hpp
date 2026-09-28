#ifndef ASCIIXEL_FRAME_CONVERTER_HPP
#define ASCIIXEL_FRAME_CONVERTER_HPP

#include "asciixel/model/image_frame.hpp"

struct AVFrame;

namespace asciixel {

ImageFrame convertFrame(const AVFrame& source, Color background);

} // namespace asciixel

#endif
