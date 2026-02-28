#ifndef SHAPE_DETECTION_CONFIG_H
#define SHAPE_DETECTION_CONFIG_H

#include "config.h"
#include "types.h"

namespace components {
    namespace shape_detection {
        // Hough line shape detection
        static constexpr const float    default_rho_resolution   = 1.0f;
        static constexpr const float    default_theta_resolution = pi / 180.0f; // pi / 180 (1 degree)
        static constexpr const int      default_threshold        = 150;
        static constexpr const uint32_t default_min_line_length  = 30U;
        static constexpr const uint32_t default_max_line_gap     = 10U;
    } // shape_detection
} // components

#endif // SHAPE_DETECTION_CONFIG_H
