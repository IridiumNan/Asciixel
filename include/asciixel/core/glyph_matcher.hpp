#ifndef GLYPH_MATCHER_HPP
#define GLYPH_MATCHER_HPP

#include "asciixel/model/ascii_charset.hpp"
#include "asciixel/model/ascii_frame.hpp"
#include "asciixel/model/image_frame.hpp"

namespace asciixel {

class GlyphMatcher {
private:
    static char matchCharacter(const Color& color, const AsciiCharset& charset);

public:
    // The input frame contains sampled linear RGB colors.
    static AsciiFrame match(const ImageFrame& frame, const AsciiCharset& charset);
};

} // namespace asciixel

#endif // GLYPH_MATCHER_HPP
