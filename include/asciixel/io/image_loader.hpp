#ifndef ASCIIXEL_IMAGE_LOADER_HPP
#define ASCIIXEL_IMAGE_LOADER_HPP

#include "asciixel/model/image_frame.hpp"

#include <string>

namespace asciixel {

// Pixels are linear RGB. Transparent pixels use the linear RGB background.
ImageFrame loadImage(const std::string& path, Color background = {0.0f, 0.0f, 0.0f});

} // namespace asciixel

#endif
