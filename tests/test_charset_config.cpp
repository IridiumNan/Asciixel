#include "asciixel/config/charset_config.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

namespace {

void require(bool condition)
{
    if (!condition) {
        throw std::runtime_error("charset config assertion failed");
    }
}

void roundTripPreservesCandidates()
{
    const std::filesystem::path path = "test-charset-config.cfg";
    const asciixel::CharsetConfig original{
        "C:/fonts/Maple Mono.ttf", 24, " =#\\'\""};
    original.save(path.string());

    const auto loaded = asciixel::CharsetConfig::load(path.string());
    require(loaded.font_path == original.font_path);
    require(loaded.pixel_size == original.pixel_size);
    require(loaded.candidates == original.candidates);
    std::filesystem::remove(path);
}

void rejectsMissingSpace()
{
    const std::filesystem::path path = "test-charset-config-invalid.cfg";
    {
        std::ofstream file(path);
        file << "font_path=font.ttf\npixel_size=24\ncandidates=#@\n";
    }
    bool rejected = false;
    try {
        asciixel::CharsetConfig::load(path.string());
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    std::filesystem::remove(path);
    require(rejected);
}

} // namespace

int main()
{
    roundTripPreservesCandidates();
    rejectsMissingSpace();
}
