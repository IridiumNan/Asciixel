#include "asciixel/core/glyph_matcher.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>

namespace asciixel {

char GlyphMatcher::matchCharacter(const Color& color, const AsciiCharset& charset)
{
    if (!std::isfinite(color.r) || !std::isfinite(color.g) || !std::isfinite(color.b) ||
        color.r < 0.0f || color.r > 1.0f || color.g < 0.0f || color.g > 1.0f ||
        color.b < 0.0f || color.b > 1.0f) {
        throw std::invalid_argument("match color must be finite linear RGB in [0, 1]");
    }

    const double brightness = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    const Glyph* best = nullptr;
    double best_distance = std::numeric_limits<double>::infinity();
    for (const Glyph& glyph : charset.glyphs) {
        const double distance = std::abs(brightness - glyph.brightness);
        if (distance < best_distance ||
            (distance == best_distance && static_cast<unsigned char>(glyph.ch) <
                                              static_cast<unsigned char>(best->ch))) {
            best = &glyph;
            best_distance = distance;
        }
    }
    return best->ch;
}

AsciiFrame GlyphMatcher::match(const SampledFrame& frame, const AsciiCharset& charset)
{
    if (frame.width == 0 || frame.height == 0 ||
        frame.width > std::numeric_limits<std::size_t>::max() / frame.height ||
        frame.pixels.size() != frame.width * frame.height) {
        throw std::invalid_argument("invalid match frame dimensions or pixel count");
    }
    if (charset.glyphs.empty()) {
        throw std::invalid_argument("match charset must not be empty");
    }
    for (const Glyph& glyph : charset.glyphs) {
        if (!std::isfinite(glyph.brightness) || glyph.brightness < 0.0f ||
            glyph.brightness > 1.0f) {
            throw std::invalid_argument("match glyph brightness must be finite and in [0, 1]");
        }
    }

    AsciiFrame result(frame.width, frame.height);
    for (std::size_t y = 0; y < frame.height; ++y) {
        for (std::size_t x = 0; x < frame.width; ++x) {
            const Color& color = frame.at(x, y).color;
            result.at(x, y) = {matchCharacter(color, charset), color};
        }
    }
    return result;
}

} // namespace asciixel
