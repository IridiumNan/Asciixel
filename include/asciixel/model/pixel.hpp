#ifndef PIXEL_HPP
#define PIXEL_HPP

#include "asciixel/model/color.hpp"

namespace asciixel {

struct ImagePixel {
    Color color;
};

struct AsciiPixel {
    char character;
    Color color;
};

} // namespace asciixel

#endif