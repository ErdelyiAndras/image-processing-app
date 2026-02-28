#ifndef HOUGH_CIRCLE_H
#define HOUGH_CIRCLE_H

#include "types.h"

namespace components {
    namespace shape_detection {
        struct HoughCircle {
            PixelIdx center_x;
            PixelIdx center_y;
            PixelIdx radius;
            uint32_t votes;
        };
    } // shape_detection
} // components

#endif // HOUGH_CIRCLE_H
