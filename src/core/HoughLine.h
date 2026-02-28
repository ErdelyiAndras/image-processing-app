#ifndef HOUGH_LINE_H
#define HOUGH_LINE_H

#include "types.h"

struct HoughLine {
    float rho;
    float theta;
    uint32_t votes;

    PixelIdx x0{ 0U };
    PixelIdx y0{ 0U };
    PixelIdx x1{ 0U };
    PixelIdx y1{ 0U };
};

#endif // HOUGH_LINE_H
