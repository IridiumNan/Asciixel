#include "asciixel/config/charset_config.hpp"

#include <charconv>
#include <fstream>
#include <stdexcept>

namespace asciixel {
namespace {

std::string readValue(std::istream& input, const std::string& key)
{
    std::string line;
    if (!std::getline(input, line)) {
        throw std::invalid_argument("Missing config field: " + key);
    }
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    const std::string prefix = key + '=';
    if (line.compare(0, prefix.size(), prefix) != 0) {
        throw std::invalid_argument("Expected config field: " + key);
    }
    return line.substr(prefix.size());
}

} // namespace

void CharsetConfig::validate() const
{
    if (font_path.empty() || font_path.find_first_of("\r\n") != std::string::npos) {
        throw std::invalid_argument("Invalid font_path");
    }
    if (pixel_size < 1 || pixel_size > 256) {
        throw std::invalid_argument("pixel_size must be 1..256");
    }
    bool seen[127] = {};
    for (unsigned char ch : candidates) {
        if (ch < 32 || ch > 126 || seen[ch]) {
            throw std::invalid_argument("candidates must be unique printable ASCII");
        }
        seen[ch] = true;
    }
    if (!seen[' ']) {
        throw std::invalid_argument("candidates must include a space");
    }
}

CharsetConfig CharsetConfig::load(const std::string& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Cannot open charset config: " + path);
    }

    CharsetConfig config;
    config.font_path             = readValue(input, "font_path");
    const std::string pixel_size = readValue(input, "pixel_size");
    const char*       begin      = pixel_size.data();
    const char*       end        = begin + pixel_size.size();
    const auto        parsed     = std::from_chars(begin, end, config.pixel_size);
    if (parsed.ec != std::errc{} || parsed.ptr != end) {
        throw std::invalid_argument("Invalid pixel_size");
    }
    config.candidates = readValue(input, "candidates");
    std::string extra;
    if (std::getline(input, extra)) {
        throw std::invalid_argument("Unexpected extra config line");
    }
    config.validate();
    return config;
}

void CharsetConfig::save(const std::string& path) const
{
    validate();
    std::ofstream output(path, std::ios::binary);
    if (!output) {
        throw std::runtime_error("Cannot write charset config: " + path);
    }
    output << "font_path=" << font_path << '\n'
           << "pixel_size=" << pixel_size << '\n'
           << "candidates=" << candidates << '\n';
    if (!output) {
        throw std::runtime_error("Cannot write charset config: " + path);
    }
}

} // namespace asciixel
