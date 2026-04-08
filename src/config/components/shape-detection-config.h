#ifndef SHAPE_DETECTION_CONFIG_H
#define SHAPE_DETECTION_CONFIG_H

#include "config.h"

namespace components {
    namespace shape_detection {
        // Hough line shape detection
        static constexpr const float default_rho_resolution   = 1.0f;
        static constexpr const float default_theta_resolution = pi / 180.0f; // pi / 180 (1 degree)
        static constexpr const int   default_threshold        = 150;
        static constexpr const int   default_min_line_length  = 30;
        static constexpr const int   default_max_line_gap     = 10;

        // Hough circle shape detection
        static constexpr const int   default_circle_vote_min_threshold = 100;
        static constexpr const int   default_min_radius                = 10;
        static constexpr const int   default_max_radius                = 100;
        static constexpr const float default_min_dist                  = 20.0f;
        static constexpr const int   default_num_angle_steps           = 360;
    } // shape_detection
} // components

#endif // SHAPE_DETECTION_CONFIG_H
