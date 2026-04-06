#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

struct Color {
    uint8_t r, g, b;

    static const Color Red;
    static const Color Green;
    static const Color Blue;
};

#endif // COLOR_H
