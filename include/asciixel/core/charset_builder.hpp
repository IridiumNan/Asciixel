#ifndef CHARSET_BUILDER_HPP
#define CHARSET_BUILDER_HPP

#include "asciixel/model/ascii_charset.hpp"
#include <string>

namespace asciixel {

class CharsetBuilder {
public:
    static AsciiCharset buildCharset(const std::string& font_path);
    static AsciiCharset loadCharset(const std::string& charset_path);

private:
};

} // namespace asciixel

#endif // CHARSET_BUILDER_HPP