#ifndef ASCIIXEL_CHARSET_CONFIG_HPP
#define ASCIIXEL_CHARSET_CONFIG_HPP

#include <string>

namespace asciixel {

struct CharsetConfig {
    std::string font_path;
    unsigned    pixel_size = 24;
    std::string candidates;

    void                 validate() const;
    static CharsetConfig load(const std::string& path);
    void                 save(const std::string& path) const;
};

} // namespace asciixel

#endif // ASCIIXEL_CHARSET_CONFIG_HPP
