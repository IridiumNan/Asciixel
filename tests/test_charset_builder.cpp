#include "asciixel/config/charset_config.hpp"
#include "asciixel/core/charset_builder.hpp"

#include <stdexcept>

void require(bool condition)
{
    if (!condition) {
        throw std::runtime_error("charset builder assertion failed");
    }
}

int main(int argc, char** argv)
{
    require(argc == 2);
    asciixel::CharsetConfig config{argv[1], 24, " A"};
    const auto charset = asciixel::CharsetBuilder::buildCharset(config);
    require(charset.glyphs.size() == 2);
    require(charset.glyphs.front().ch == ' ');
    require(charset.glyphs.front().brightness == 0);
    require(charset.glyphs.back().ch == 'A');
    require(charset.glyphs.back().brightness == 1);

    config.pixel_size = 0;
    bool rejected = false;
    try {
        asciixel::CharsetBuilder::buildCharset(config);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    require(rejected);
}
