#ifndef HOUGH_CIRCLE_SHAPE_DETECTION_PARAMETERS_H
#define HOUGH_CIRCLE_SHAPE_DETECTION_PARAMETERS_H

#include "Parameters.h"
#include "types.h"

namespace components {
    namespace shape_detection {
        struct HoughCircleShapeDetectionParameters : public Parameters {
            uint32_t vote_min_threshold;
            uint32_t min_radius;
            uint32_t max_radius;
            float min_dist;

            HoughCircleShapeDetectionParameters(
                uint32_t vote_min_threshold,
                uint32_t min_radius,
                uint32_t max_radius,
                float min_dist
            )
                : vote_min_threshold(vote_min_threshold)
                , min_radius(min_radius)
                , max_radius(max_radius)
                , min_dist(min_dist) {}
        };
    } // shape_detection
} // components

#endif // HOUGH_CIRCLE_SHAPE_DETECTION_PARAMETERS_H
