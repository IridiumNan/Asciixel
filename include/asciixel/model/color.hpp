#ifndef COLOR_HPP
#define COLOR_HPP

namespace asciixel {

struct Color {
    float r;
    float g;
    float b;

    Color operator+(const Color& other) const
    {
        return {r + other.r, g + other.g, b + other.b};
    }
};

}// namespace asciixel

#endif