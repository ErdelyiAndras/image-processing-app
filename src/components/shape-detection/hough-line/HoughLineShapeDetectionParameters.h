#ifndef HOUGH_LINE_SHAPE_DETECTION_PARAMETERS_H
#define HOUGH_LINE_SHAPE_DETECTION_PARAMETERS_H

#include "Parameters.h"
#include "types.h"

namespace components {
    namespace shape_detection {
        struct HoughLineShapeDetectionParameters : public Parameters {
            float rho_resolution;
            float theta_resolution;
            uint32_t vote_min_threshold;
            uint32_t min_line_length;
            uint32_t max_line_gap;

            HoughLineShapeDetectionParameters(
                float rho_resolution,
                float theta_resolution,
                uint32_t vote_min_threshold,
                uint32_t min_line_length,
                uint32_t max_line_gap
            )
                : rho_resolution(rho_resolution)
                , theta_resolution(theta_resolution)
                , vote_min_threshold(vote_min_threshold)
                , min_line_length(min_line_length)
                , max_line_gap(max_line_gap) {}
        };
    }
}

#endif // HOUGH_LINE_SHAPE_DETECTION_PARAMETERS_H
