#include "asciixel/core/image_sampler.hpp"

#include <cmath>
#include <stdexcept>

namespace {

using asciixel::Color;
using asciixel::ImageFrame;
using asciixel::ImagePixel;
using asciixel::ImageSampler;

bool nearlyEqual(float left, float right)
{
    return std::fabs(left - right) < 0.0001f;
}

void require(bool condition)
{
    if (!condition) {
        throw std::runtime_error("unit test assertion failed");
    }
}

void assertColor(const Color& color, float r, float g, float b)
{
    require(nearlyEqual(color.r, r));
    require(nearlyEqual(color.g, g));
    require(nearlyEqual(color.b, b));
}

void fillFrame(ImageFrame& frame, const Color& color)
{
    for (auto& pixel : frame.pixels) {
        pixel.color = color;
    }
}

void sampleConstantFrame()
{
    ImageFrame frame(4, 2);
    fillFrame(frame, {0.2f, 0.4f, 0.8f});

    const ImageFrame sampled = ImageSampler::sample(frame, 2, 1);

    require(sampled.width == 2);
    require(sampled.height == 1);
    assertColor(sampled.at(0, 0).color, 0.2f, 0.4f, 0.8f);
    assertColor(sampled.at(1, 0).color, 0.2f, 0.4f, 0.8f);
}

void preservePixelsWhenDimensionsMatch()
{
    ImageFrame frame(2, 2);
    frame.at(0, 0) = ImagePixel{{1.0f, 0.0f, 0.0f}};
    frame.at(1, 0) = ImagePixel{{0.0f, 1.0f, 0.0f}};
    frame.at(0, 1) = ImagePixel{{0.0f, 0.0f, 1.0f}};
    frame.at(1, 1) = ImagePixel{{1.0f, 1.0f, 1.0f}};

    const ImageFrame sampled = ImageSampler::sample(frame, 2, 2);

    assertColor(sampled.at(0, 0).color, 1.0f, 0.0f, 0.0f);
    assertColor(sampled.at(1, 0).color, 0.0f, 1.0f, 0.0f);
    assertColor(sampled.at(0, 1).color, 0.0f, 0.0f, 1.0f);
    assertColor(sampled.at(1, 1).color, 1.0f, 1.0f, 1.0f);
}

void rejectZeroDimensions()
{
    const ImageFrame frame(2, 2);

    bool threw = false;
    try {
        ImageSampler::sample(frame, 0, 1);
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    require(threw);
}

} // namespace

int main()
{
    sampleConstantFrame();
    preservePixelsWhenDimensionsMatch();
    rejectZeroDimensions();
}