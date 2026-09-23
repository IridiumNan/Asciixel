#ifndef ASCII_CHARSET_HPP
#define ASCII_CHARSET_HPP

#include <vector>

namespace asciixel {

struct Glyph{
    char ch;
    float brightness;
};

struct AsciiCharset {
    std::vector<Glyph> glyphs;
};

} // namespace asciixel

#endif