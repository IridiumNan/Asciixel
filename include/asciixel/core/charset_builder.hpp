#ifndef CHARSET_BUILDER_HPP
#define CHARSET_BUILDER_HPP

#include "asciixel/config/charset_config.hpp"
#include "asciixel/model/ascii_charset.hpp"

namespace asciixel {

class CharsetBuilder {
public:
    static AsciiCharset buildCharset(const CharsetConfig& config);
};

} // namespace asciixel

#endif // CHARSET_BUILDER_HPP
