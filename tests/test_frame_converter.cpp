#include "asciixel/io/frame_converter.hpp"

extern "C" {
#include <libavutil/frame.h>
#include <libavutil/pixfmt.h>
}

#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace {

void require(bool condition, const char* message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void requireNear(float actual, float expected, const char* message)
{
    require(std::fabs(actual - expected) < 0.001f, message);
}

void preservesDimensionsAndConvertsSrgbToLinear()
{
    std::uint8_t pixels[] = {255, 0, 0, 255, 128, 128, 128, 255};
    AVFrame source{};
    source.width = 2;
    source.height = 1;
    source.format = AV_PIX_FMT_RGBA;
    source.data[0] = pixels;
    source.linesize[0] = sizeof(pixels);

    const auto result = asciixel::convertFrame(source, {0.0f, 0.0f, 0.0f});
    require(result.width == 2 && result.height == 1, "frame dimensions changed");
    require(result.pixels.size() == 2, "frame pixel count changed");
    requireNear(result.at(0, 0).color.r, 1.0f, "red channel changed");
    requireNear(result.at(0, 0).color.g, 0.0f, "green channel changed");
    requireNear(result.at(0, 0).color.b, 0.0f, "blue channel changed");
    requireNear(result.at(1, 0).color.r, 0.215861f, "sRGB red was not linearized");
    requireNear(result.at(1, 0).color.g, 0.215861f, "sRGB green was not linearized");
    requireNear(result.at(1, 0).color.b, 0.215861f, "sRGB blue was not linearized");
}

void compositesTransparentPixelsOverLinearBackground()
{
    std::uint8_t pixels[] = {0, 0, 255, 128, 255, 0, 0, 0};
    AVFrame source{};
    source.width = 2;
    source.height = 1;
    source.format = AV_PIX_FMT_RGBA;
    source.data[0] = pixels;
    source.linesize[0] = sizeof(pixels);

    const auto result = asciixel::convertFrame(source, {0.2f, 0.4f, 0.6f});
    requireNear(result.at(0, 0).color.r, 0.099608f, "half-transparent red blend is wrong");
    requireNear(result.at(0, 0).color.g, 0.199216f, "half-transparent green blend is wrong");
    requireNear(result.at(0, 0).color.b, 0.800784f, "half-transparent blue blend is wrong");
    requireNear(result.at(1, 0).color.r, 0.2f, "transparent red should equal background");
    requireNear(result.at(1, 0).color.g, 0.4f, "transparent green should equal background");
    requireNear(result.at(1, 0).color.b, 0.6f, "transparent blue should equal background");
}

void rejectsUnsupportedPixelFormat()
{
    AVFrame source{};
    source.width = 1;
    source.height = 1;
    source.format = AV_PIX_FMT_NONE;

    bool threw = false;
    try {
        asciixel::convertFrame(source, {0.0f, 0.0f, 0.0f});
    } catch (const std::runtime_error&) {
        threw = true;
    }
    require(threw, "unsupported pixel format should raise runtime_error");
}

} // namespace

int main()
{
    preservesDimensionsAndConvertsSrgbToLinear();
    compositesTransparentPixelsOverLinearBackground();
    rejectsUnsupportedPixelFormat();
}
