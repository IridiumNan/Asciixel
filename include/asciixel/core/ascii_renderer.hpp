#ifndef ASCIIXEL_ASCII_RENDERER_HPP
#define ASCIIXEL_ASCII_RENDERER_HPP
#include "asciixel/model/ascii_frame.hpp"
#include "asciixel/model/gray_bitmap.hpp"
#include "asciixel/model/rasterized_charset.hpp"
namespace asciixel {
// Render white glyphs on black; invalid/missing glyph data throws invalid_argument.
GrayBitmap renderAscii(const AsciiFrame& frame, const RasterizedCharset& charset);
}
#endif
