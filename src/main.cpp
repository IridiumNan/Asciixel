#include "asciixel/core/charset_builder.hpp"
#include "asciixel/core/glyph_matcher.hpp"
#include "asciixel/core/grid_layout.hpp"
#include "asciixel/core/image_sampler.hpp"
#include "asciixel/io/image_loader.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#endif

namespace {

const char* defaultFontPath()
{
#ifdef _WIN32
    return "C:/Windows/Fonts/consola.ttf";
#else
    return "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf";
#endif
}

asciixel::CharsetConfig defaultCharsetConfig()
{
    asciixel::CharsetConfig config;
    config.font_path = defaultFontPath();
    config.pixel_size = 24;
    for (int ch = 32; ch <= 126; ++ch) {
        config.candidates += static_cast<char>(ch);
    }
    return config;
}

// TODO: Move text export into a dedicated output module.
void writeAsciiFrame(const asciixel::AsciiFrame& frame)
{
#ifdef _WIN32
    if (_setmode(_fileno(stdout), _O_BINARY) == -1) {
        throw std::runtime_error("Cannot set stdout to binary mode");
    }
#endif
    for (std::size_t y = 0; y < frame.height; ++y) {
        for (std::size_t x = 0; x < frame.width; ++x) {
            std::cout.put(frame.pixels[y * frame.width + x].character);
        }
        std::cout.put('\n');
    }
    if (!std::cout) {
        throw std::runtime_error("Cannot write ASCII frame");
    }
}

#ifdef _WIN32
std::string toUtf8(const wchar_t* value)
{
    const int length = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
                                           value, -1, nullptr, 0, nullptr, nullptr);
    if (length == 0) {
        throw std::runtime_error("Cannot encode image path as UTF-8");
    }
    std::string utf8(static_cast<std::size_t>(length), '\0');
    if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, value, -1,
                            utf8.data(), length, nullptr, nullptr) == 0) {
        throw std::runtime_error("Cannot encode image path as UTF-8");
    }
    utf8.pop_back();
    return utf8;
}
#endif

void convertImage(const std::string& path)
{
    const asciixel::ImageFrame image = asciixel::loadImage(path);
    const asciixel::GridSize grid = asciixel::calculateGrid(image.width, image.height);
    const asciixel::ImageFrame sampled =
        asciixel::ImageSampler::sample(image, grid.columns, grid.rows);
    const asciixel::AsciiCharset charset =
        asciixel::CharsetBuilder::buildCharset(defaultCharsetConfig());
    writeAsciiFrame(asciixel::GlyphMatcher::match(sampled, charset));
}

} // namespace

#ifdef _WIN32
int wmain(int argc, wchar_t** argv)
#else
int main(int argc, char** argv)
#endif
{
    if (argc != 2) {
        std::cerr << "Usage: asciixel <image-path>\n";
        return 2;
    }

    try {
#ifdef _WIN32
        convertImage(toUtf8(argv[1]));
#else
        convertImage(argv[1]);
#endif
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
