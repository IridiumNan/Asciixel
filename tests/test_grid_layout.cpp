#include "asciixel/core/grid_layout.hpp"
#include <limits>
#include <stdexcept>

void require(bool condition) {
    if (!condition) throw std::runtime_error("grid layout assertion failed");
}
int main() {
    const asciixel::GlyphLayout half{10, 20, 0, 15};
    const auto wide = asciixel::calculateGrid(400, 200, {200}, half);
    require(wide.columns == 200 && wide.rows == 50);
    const auto square = asciixel::calculateGrid(400, 200, {120}, {20, 20, 0, 15});
    require(square.columns == 120 && square.rows == 60);
    const auto narrow = asciixel::calculateGrid(400, 200, {120}, {10, 30, 0, 20});
    require(narrow.columns == 120 && narrow.rows == 20);
    const auto small = asciixel::calculateGrid(20, 10, {200}, half);
    require(small.columns == 20 && small.rows == 5);
    require(asciixel::calculateGrid(1, 5, {200}, half).rows == 3);
    require(asciixel::calculateGrid(1000, 1, {1}, half).rows == 1);
    auto rejects = [&](std::size_t w, std::size_t h, std::size_t columns,
                       asciixel::GlyphLayout layout) {
        bool rejected = false;
        try { asciixel::calculateGrid(w, h, {columns}, layout); }
        catch (const std::invalid_argument&) { rejected = true; }
        require(rejected);
    };
    rejects(0, 10, 200, half);
    rejects(10, 0, 200, half);
    rejects(10, 10, 0, half);
    rejects(10, 10, 200, {0, 20, 0, 15});
    rejects(10, 10, 200, {10, 0, 0, 15});
    rejects(2, std::numeric_limits<std::size_t>::max(), 2, {2, 1, 0, 0});
    rejects(2, std::numeric_limits<std::size_t>::max() / 2 + 1, 2, {1, 1, 0, 0});
}
