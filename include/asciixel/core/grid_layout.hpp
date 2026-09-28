#ifndef GRID_LAYOUT_HPP
#define GRID_LAYOUT_HPP

#include <cstddef>

namespace asciixel {

struct GridSize {
    std::size_t columns;
    std::size_t rows;
};

// Cap the column count at 200. Scale rows by the same factor, then halve them
// to account for character cells that are about twice as tall as they are wide.
GridSize calculateGrid(std::size_t image_width, std::size_t image_height);

} // namespace asciixel

#endif // GRID_LAYOUT_HPP
