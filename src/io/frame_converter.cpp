#include "asciixel/io/frame_converter.hpp"

extern "C" {
#include <libavutil/frame.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
}

#include <cmath>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

namespace asciixel {
namespace {

float toLinear(std::uint8_t channel)
{
    const float value = channel / 255.0f;
    return value <= 0.04045f ? value / 12.92f : std::pow((value + 0.055f) / 1.055f, 2.4f);
}

} // namespace

ImageFrame convertFrame(const AVFrame& source, Color background)
{
    if (!av_pix_fmt_desc_get(static_cast<AVPixelFormat>(source.format))) {
        throw std::runtime_error("Cannot convert image pixels");
    }

    std::unique_ptr<SwsContext, decltype(&sws_freeContext)> scaler(
        sws_getContext(source.width, source.height, static_cast<AVPixelFormat>(source.format),
                       source.width, source.height, AV_PIX_FMT_RGBA,
                       SWS_BILINEAR, nullptr, nullptr, nullptr),
        sws_freeContext);
    if (!scaler) {
        throw std::runtime_error("Cannot convert image pixels");
    }

    const std::size_t width = static_cast<std::size_t>(source.width);
    const std::size_t height = static_cast<std::size_t>(source.height);
    std::vector<std::uint8_t> rgba(width * height * 4);
    std::uint8_t* output[] = {rgba.data(), nullptr, nullptr, nullptr};
    int stride[] = {source.width * 4, 0, 0, 0};
    if (sws_scale(scaler.get(), source.data, source.linesize, 0, source.height,
                  output, stride) != source.height) {
        throw std::runtime_error("Cannot convert image pixels");
    }

    ImageFrame image(width, height);
    for (std::size_t i = 0; i < image.pixels.size(); ++i) {
        const auto* pixel = rgba.data() + i * 4;
        const float alpha = pixel[3] / 255.0f;
        image.pixels[i].color = {
            alpha * toLinear(pixel[0]) + (1.0f - alpha) * background.r,
            alpha * toLinear(pixel[1]) + (1.0f - alpha) * background.g,
            alpha * toLinear(pixel[2]) + (1.0f - alpha) * background.b,
        };
    }
    return image;
}

} // namespace asciixel
