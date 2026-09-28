#include "asciixel/core/glyph_matcher.hpp"

#include <stdexcept>
#include <initializer_list>
#include <utility>

namespace {

asciixel::RasterizedCharset makeCharset(std::initializer_list<std::pair<char, float>> entries)
{
    asciixel::RasterizedCharset result;
    for (const auto& entry : entries) {
        asciixel::RasterizedGlyph glyph;
        glyph.character = entry.first;
        glyph.density = entry.second;
        result.glyphs.push_back(glyph);
    }
    return result;
}

void require(bool condition)
{
    if (!condition) {
        throw std::runtime_error("glyph matcher assertion failed");
    }
}

void matchLinearRgbBrightnessAcrossFrame()
{
    const auto charset = makeCharset({{' ', 0.0f}, {'+', 0.15f}, {'#', 0.25f}});
    asciixel::SampledFrame frame(2, 2);
    frame.at(0, 0).color = {1.0f, 0.0f, 0.0f};
    frame.at(1, 0).color = {0.0f, 1.0f, 0.0f};
    frame.at(0, 1).color = {0.0f, 0.0f, 1.0f};
    frame.at(1, 1).color = {1.0f, 1.0f, 1.0f};

    auto result = asciixel::GlyphMatcher::match(frame, charset);
    require(result.width == 2 && result.height == 2);
    require(result.at(0, 0).character == ' ');
    require(result.at(1, 0).character == '+');
    require(result.at(0, 1).character == ' ');
    require(result.at(1, 1).character == '#');
}

void breakEqualDistanceTiesByCharacter()
{
    const auto charset = makeCharset({{'Z', 0.0f}, {'A', 0.5f}});
    asciixel::SampledFrame frame(1, 1);
    frame.at(0, 0).color = {0.5f, 0.5f, 0.5f};

    require(asciixel::GlyphMatcher::match(frame, charset).at(0, 0).character == 'A');
}

void rejectEmptyCharset()
{
    const asciixel::RasterizedCharset charset;
    asciixel::SampledFrame frame(1, 1);
    frame.at(0, 0).color = {0.0f, 0.0f, 0.0f};

    bool rejected = false;
    try {
        asciixel::GlyphMatcher::match(frame, charset);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    require(rejected);
}

void stretchNonzeroDensityRangeWithoutChangingCharset()
{
    const auto charset = makeCharset({{'A', 0.125f}, {'B', 0.375f}});
    asciixel::SampledFrame frame(2, 1);
    frame.at(0, 0).color = {0, 0, 0};
    frame.at(1, 0).color = {1, 1, 1};
    const auto result = asciixel::GlyphMatcher::match(frame, charset);
    require(result.pixels[0].character == 'A');
    require(result.pixels[1].character == 'B');
    require(charset.glyphs[0].density == 0.125f && charset.glyphs[1].density == 0.375f);
}

void handleUniformDensity()
{
    const auto charset = makeCharset({{'Z', 0.25f}, {'A', 0.25f}});
    asciixel::SampledFrame frame(1, 1);
    frame.at(0, 0).color = {1, 1, 1};
    require(asciixel::GlyphMatcher::match(frame, charset).pixels[0].character == 'A');
}

} // namespace

int main()
{
    matchLinearRgbBrightnessAcrossFrame();
    breakEqualDistanceTiesByCharacter();
    rejectEmptyCharset();
    stretchNonzeroDensityRangeWithoutChangingCharset();
    handleUniformDensity();
}
