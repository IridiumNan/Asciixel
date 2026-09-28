#include "asciixel/core/glyph_matcher.hpp"

#include <cmath>
#include <algorithm>
#include <limits>
#include <stdexcept>

namespace asciixel {

char GlyphMatcher::matchCharacter(const Color& color, const RasterizedCharset& charset,
                                 double minimum_density, double density_range)
{
    if (!std::isfinite(color.r) || !std::isfinite(color.g) || !std::isfinite(color.b) ||
        color.r < 0.0f || color.r > 1.0f || color.g < 0.0f || color.g > 1.0f ||
        color.b < 0.0f || color.b > 1.0f) {
        throw std::invalid_argument("match color must be finite linear RGB in [0, 1]");
    }

    const double brightness = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    const RasterizedGlyph* best = nullptr;
    double best_distance = std::numeric_limits<double>::infinity();
    for (const RasterizedGlyph& glyph : charset.glyphs) {
        const double level = density_range > 0 ?
            (glyph.density - minimum_density) / density_range : 0.0;
        const double distance = std::abs(brightness - level);
        if (distance < best_distance ||
            (distance == best_distance && static_cast<unsigned char>(glyph.character) <
                                              static_cast<unsigned char>(best->character))) {
            best = &glyph;
            best_distance = distance;
        }
    }
    return best->character;
}

AsciiFrame GlyphMatcher::match(const SampledFrame& frame, const RasterizedCharset& charset)
{
    if (frame.width == 0 || frame.height == 0 ||
        frame.width > std::numeric_limits<std::size_t>::max() / frame.height ||
        frame.pixels.size() != frame.width * frame.height) {
        throw std::invalid_argument("invalid match frame dimensions or pixel count");
    }
    if (charset.glyphs.empty()) {
        throw std::invalid_argument("match charset must not be empty");
    }
    double minimum = 1.0;
    double maximum = 0.0;
    for (const RasterizedGlyph& glyph : charset.glyphs) {
        if (!std::isfinite(glyph.density) || glyph.density < 0.0f ||
            glyph.density > 1.0f) {
            throw std::invalid_argument("match glyph density must be finite and in [0, 1]");
        }
        minimum = std::min(minimum, static_cast<double>(glyph.density));
        maximum = std::max(maximum, static_cast<double>(glyph.density));
    }

    AsciiFrame result(frame.width, frame.height);
    for (std::size_t y = 0; y < frame.height; ++y) {
        for (std::size_t x = 0; x < frame.width; ++x) {
            const Color& color = frame.at(x, y).color;
            result.at(x, y) = {matchCharacter(color, charset, minimum, maximum - minimum), color};
        }
    }
    return result;
}

} // namespace asciixel
