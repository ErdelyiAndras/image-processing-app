#ifndef EDGE_DETECTION_CONFIG_H
#define EDGE_DETECTION_CONFIG_H

namespace components {
    namespace edge_detection {
        // Sobel edge detection
        static constexpr const float default_threshold = 0.1f;

        // Canny edge detection
        static constexpr const float default_low_threshold = 0.05f;
        static constexpr const float default_high_threshold = 0.15f;

        static constexpr const float strong = 1.0f;
        static constexpr const float weak = 0.25f;
    } // edge_detection
} // components

#endif // EDGE_DETECTION_CONFIG_H
