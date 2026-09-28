#include "asciixel/io/png_writer.hpp"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

void require(bool value)
{
    if (!value) throw std::runtime_error("PNG writer assertion failed");
}
int main(int argc, char** argv)
try {
    require(argc == 2);
    const auto path = std::filesystem::u8path(argv[1]);
    std::filesystem::remove(path);
    asciixel::GrayBitmap bitmap{3, 2, {0, 128, 255, 255, 64, 0}};
    asciixel::writePng(bitmap, argv[1]);
    std::ifstream                    file(path, std::ios::binary);
    const std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(file)), {});
    require(bytes.size() > 26 && bytes[0] == 137 && bytes[1] == 'P');
    require(bytes[24] == 8 && bytes[25] == 0); // 8-bit grayscale, no alpha channel.
    file.close();
    bool rejected = false;
    try {
        asciixel::writePng(bitmap, argv[1]);
    }
    catch (const std::runtime_error&) {
        rejected = true;
    }
    require(rejected);
    require(std::filesystem::file_size(path) == bytes.size());
    AVFormatContext* input = nullptr;
    require(avformat_open_input(&input, argv[1], nullptr, nullptr) >= 0);
    require(avformat_find_stream_info(input, nullptr) >= 0);
    const auto*     codec   = avcodec_find_decoder(AV_CODEC_ID_PNG);
    AVCodecContext* decoder = avcodec_alloc_context3(codec);
    require(decoder && avcodec_open2(decoder, codec, nullptr) >= 0);
    AVPacket* packet = av_packet_alloc();
    AVFrame*  frame  = av_frame_alloc();
    require(av_read_frame(input, packet) >= 0);
    require(avcodec_send_packet(decoder, packet) >= 0);
    require(avcodec_receive_frame(decoder, frame) >= 0);
    require(frame->width == 3 && frame->height == 2 && frame->format == AV_PIX_FMT_GRAY8);
    for (int y = 0; y < 2; ++y)
        for (int x = 0; x < 3; ++x)
            require(frame->data[0][y * frame->linesize[0] + x] == bitmap.pixels[y * 3 + x]);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&decoder);
    avformat_close_input(&input);
    std::filesystem::remove(path);
    bitmap.pixels.pop_back();
    rejected = false;
    try {
        asciixel::writePng(bitmap, argv[1]);
    }
    catch (const std::invalid_argument&) {
        rejected = true;
    }
    require(rejected && !std::filesystem::exists(path));
}
catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
