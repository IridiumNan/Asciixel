#include "asciixel/io/png_writer.hpp"
extern "C" {
#include <libavcodec/avcodec.h>
}
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <limits>
#include <memory>
#include <stdexcept>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#endif

namespace asciixel {
namespace {
void freeEncoder(AVCodecContext* p)
{ avcodec_free_context(&p); }

void freeFrame(AVFrame* p)
{ av_frame_free(&p); }

void freePacket(AVPacket* p)
{ av_packet_free(&p); }
} // namespace

void writePng(const GrayBitmap& bitmap, const std::string& path)
{
    if (!bitmap.width || !bitmap.height || bitmap.width > maxBitmapPixels / bitmap.height || bitmap.width > static_cast<std::size_t>(std::numeric_limits<int>::max()) || bitmap.height > static_cast<std::size_t>(std::numeric_limits<int>::max()) || bitmap.pixels.size() != bitmap.width * bitmap.height)
        throw std::invalid_argument("Invalid or excessive grayscale bitmap size");
    const auto* codec = avcodec_find_encoder(AV_CODEC_ID_PNG);
    if (!codec) throw std::runtime_error("PNG encoder unavailable; rebuild FFmpeg with --enable-encoder=png");
    std::unique_ptr<AVCodecContext, decltype(&freeEncoder)> encoder(avcodec_alloc_context3(codec), freeEncoder);
    std::unique_ptr<AVFrame, decltype(&freeFrame)>          frame(av_frame_alloc(), freeFrame);
    std::unique_ptr<AVPacket, decltype(&freePacket)>        packet(av_packet_alloc(), freePacket);
    if (!encoder || !frame || !packet) throw std::runtime_error("Cannot allocate PNG encoder buffers");
    encoder->width     = static_cast<int>(bitmap.width);
    encoder->height    = static_cast<int>(bitmap.height);
    encoder->pix_fmt   = AV_PIX_FMT_GRAY8;
    encoder->time_base = {1, 1};
    if (avcodec_open2(encoder.get(), codec, nullptr) < 0)
        throw std::runtime_error("Cannot open PNG encoder");
    frame->width  = encoder->width;
    frame->height = encoder->height;
    frame->format = encoder->pix_fmt;
    frame->pts    = 0;
    if (av_frame_get_buffer(frame.get(), 0) < 0)
        throw std::runtime_error("Cannot allocate PNG frame");
    for (std::size_t y = 0; y < bitmap.height; ++y)
        std::copy_n(bitmap.pixels.data() + y * bitmap.width, bitmap.width,
                    frame->data[0] + y * static_cast<std::size_t>(frame->linesize[0]));
    if (avcodec_send_frame(encoder.get(), frame.get()) < 0 || avcodec_receive_packet(encoder.get(), packet.get()) < 0)
        throw std::runtime_error("Cannot encode PNG image");

    const auto destination = std::filesystem::u8path(path);
#ifdef _WIN32
    // Older Windows CRTs do not support fopen's C11 'x' mode.
    const int descriptor = _wopen(destination.c_str(), _O_WRONLY | _O_CREAT | _O_EXCL | _O_BINARY,
                                  _S_IREAD | _S_IWRITE);
    if (descriptor < 0)
        throw std::runtime_error("Cannot create PNG (file may already exist): " + path);
    std::FILE* raw = _fdopen(descriptor, "wb");
    if (!raw) {
        _close(descriptor);
        std::error_code ignored;
        std::filesystem::remove(destination, ignored);
        throw std::runtime_error("Cannot open PNG output stream: " + path);
    }
#else
    std::FILE* raw = std::fopen(destination.c_str(), "wbx");
#endif
    if (!raw) throw std::runtime_error("Cannot create PNG (file may already exist): " + path);
    std::unique_ptr<std::FILE, decltype(&std::fclose)> file(raw, std::fclose);
    const bool                                         written = std::fwrite(packet->data, 1, packet->size, file.get()) == static_cast<std::size_t>(packet->size);
    const bool                                         closed  = std::fclose(file.release()) == 0;
    if (!written || !closed) {
        std::error_code ignored;
        std::filesystem::remove(destination, ignored);
        throw std::runtime_error("Cannot write PNG: " + path);
    }
}
} // namespace asciixel
