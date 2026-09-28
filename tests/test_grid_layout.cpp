#include "asciixel/core/grid_layout.hpp"

#include <stdexcept>

namespace {

void require(bool condition)
{
    if (!condition) {
        throw std::runtime_error("grid layout assertion failed");
    }
}

void capColumnsAtTwoHundredAndPreserveAspect()
{
    const auto landscape = asciixel::calculateGrid(200, 100);
    require(landscape.columns == 200 && landscape.rows == 50);

    const auto portrait = asciixel::calculateGrid(100, 200);
    require(portrait.columns == 100 && portrait.rows == 100);

    const auto tall = asciixel::calculateGrid(200, 1000);
    require(tall.columns == 200 && tall.rows == 500);

    const auto wide = asciixel::calculateGrid(400, 200);
    require(wide.columns == 200 && wide.rows == 50);
}

void keepSmallImagesAtTheirSourceScale()
{
    const auto small = asciixel::calculateGrid(20, 10);
    require(small.columns == 20 && small.rows == 5);

    const auto tiny = asciixel::calculateGrid(1, 5);
    require(tiny.columns == 1 && tiny.rows == 3);
}

void rejectEmptyDimensions()
{
    bool rejected = false;
    try {
        asciixel::calculateGrid(0, 10);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    require(rejected);
}

} // namespace

int main()
{
    capColumnsAtTwoHundredAndPreserveAspect();
    keepSmallImagesAtTheirSourceScale();
    rejectEmptyDimensions();
}
