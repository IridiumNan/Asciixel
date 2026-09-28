#ifndef ASCIIXEL_PNG_WRITER_HPP
#define ASCIIXEL_PNG_WRITER_HPP
#include "asciixel/model/gray_bitmap.hpp"
#include <string>
namespace asciixel {
// UTF-8 path; creates a new file exclusively and never overwrites existing files.
void writePng(const GrayBitmap& bitmap, const std::string& path);
}
#endif
