#include "asciixel/core/grid_layout.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace asciixel {

GridSize calculateGrid(std::size_t image_width, std::size_t image_height)
{
    if (image_width == 0 || image_height == 0) {
        throw std::invalid_argument("image dimensions must be greater than zero");
    }

    const std::size_t columns = std::min<std::size_t>(200, image_width);
    const double scale = static_cast<double>(columns) / static_cast<double>(image_width);
    const auto rows = static_cast<std::size_t>(std::round(image_height * scale / 2.0));
    return {columns, std::max<std::size_t>(1, rows)};
}

} // namespace asciixel
