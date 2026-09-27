#include "asciixel/core/charset_builder.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>

namespace asciixel {
namespace {

using LibraryHandle = std::unique_ptr<FT_LibraryRec_, decltype(&FT_Done_FreeType)>;
using FaceHandle    = std::unique_ptr<FT_FaceRec_, decltype(&FT_Done_Face)>;

struct Font {
    LibraryHandle library;
    FaceHandle    face;
};

Font loadFont(const std::string& font_path, unsigned pixel_size)
{
    FT_Library raw_library = nullptr;
    if (FT_Init_FreeType(&raw_library)) {
        throw std::runtime_error("FreeType initialization failed");
    }
    LibraryHandle library(raw_library, FT_Done_FreeType);

    FT_Face raw_face = nullptr;
    if (FT_New_Face(library.get(), font_path.c_str(), 0, &raw_face)) {
        throw std::runtime_error("Cannot load font: " + font_path);
    }
    FaceHandle face(raw_face, FT_Done_Face);
    if (FT_Set_Pixel_Sizes(face.get(), 0, pixel_size)) {
        throw std::runtime_error("Cannot set font size");
    }
    return {std::move(library), std::move(face)};
}

struct BitmapView {
    const unsigned char* pixels;
    unsigned int         width;
    unsigned int         rows;
    int                  pitch;
};

// The pixels belong to face and remain valid until the next FT_Load_Char call.
BitmapView getBitmap(FT_Face face, int ch)
{
    if (!FT_Get_Char_Index(face, ch) || FT_Load_Char(face, ch, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL)) {
        throw std::runtime_error("Cannot render ASCII character: " + std::to_string(ch));
    }
    const FT_Bitmap& bitmap = face->glyph->bitmap;
    if (bitmap.width && bitmap.rows && bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
        throw std::runtime_error("Expected grayscale glyph bitmap");
    }
    return {bitmap.buffer, bitmap.width, bitmap.rows, bitmap.pitch};
}

float calculateBrightness(const BitmapView& bitmap, long cell_width, long cell_height)
{
    double sum = 0;
    for (unsigned int y = 0; y < bitmap.rows; ++y) {
        const unsigned char* row = bitmap.pitch >= 0 ? bitmap.pixels + y * bitmap.pitch : bitmap.pixels + (bitmap.rows - 1 - y) * (-bitmap.pitch);
        for (unsigned int x = 0; x < bitmap.width; ++x) {
            sum += row[x];
        }
    }
    return static_cast<float>(sum / (255.0 * cell_width * cell_height));
}

void normalizeBrightness(AsciiCharset& charset)
{
    if (charset.glyphs.empty()) {
        return;
    }
    float minimum = charset.glyphs.front().brightness;
    float maximum = minimum;
    for (const Glyph& glyph : charset.glyphs) {
        minimum = std::min(minimum, glyph.brightness);
        maximum = std::max(maximum, glyph.brightness);
    }
    const float range = maximum - minimum;
    for (Glyph& glyph : charset.glyphs) {
        glyph.brightness = range > 0 ? (glyph.brightness - minimum) / range : 0;
    }
}

} // namespace

AsciiCharset CharsetBuilder::buildCharset(const CharsetConfig& config)
{
    config.validate();
    Font         font = loadFont(config.font_path, config.pixel_size);
    FT_Face      face = font.face.get();
    AsciiCharset charset;
    // Every glyph uses the same cell area, including the space.
    if (FT_Load_Char(face, ' ', FT_LOAD_DEFAULT)) {
        throw std::runtime_error("Cannot measure character cell");
    }
    const long width  = face->glyph->advance.x / 64;
    const long height = face->size->metrics.height / 64;
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Invalid font cell size");
    }
    for (unsigned char ch : config.candidates) {
        const BitmapView bitmap = getBitmap(face, ch);
        charset.glyphs.push_back({static_cast<char>(ch),
                                  calculateBrightness(bitmap, width, height)});
    }
    normalizeBrightness(charset);
    std::sort(charset.glyphs.begin(), charset.glyphs.end(),
              [](const Glyph& a, const Glyph& b) {
                  return a.brightness == b.brightness ? a.ch < b.ch : a.brightness < b.brightness;
              });
    return charset;
}

} // namespace asciixel
