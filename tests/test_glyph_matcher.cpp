#include "asciixel/core/glyph_matcher.hpp"

#include <stdexcept>

namespace {

void require(bool condition)
{
    if (!condition) {
        throw std::runtime_error("glyph matcher assertion failed");
    }
}

void matchLinearRgbBrightnessAcrossFrame()
{
    const asciixel::AsciiCharset charset{{{' ', 0.0f}, {'+', 0.6f}, {'#', 1.0f}}};
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
    const asciixel::AsciiCharset charset{{{'Z', 0.0f}, {'A', 0.5f}}};
    asciixel::SampledFrame frame(1, 1);
    frame.at(0, 0).color = {0.25f, 0.25f, 0.25f};

    require(asciixel::GlyphMatcher::match(frame, charset).at(0, 0).character == 'A');
}

void rejectEmptyCharset()
{
    const asciixel::AsciiCharset charset;
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

} // namespace

int main()
{
    matchLinearRgbBrightnessAcrossFrame();
    breakEqualDistanceTiesByCharacter();
    rejectEmptyCharset();
}
