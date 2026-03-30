#ifndef HOUGH_CIRCLE_SHAPE_DETECTION_PARAMETERS_H
#define HOUGH_CIRCLE_SHAPE_DETECTION_PARAMETERS_H

#include "Parameters.h"
#include "shape-detection-config.h"
#include "types.h"

namespace components {
    namespace shape_detection {
        struct HoughCircleShapeDetectionParameters : public Parameters {
            uint32_t vote_min_threshold;
            uint32_t min_radius;
            uint32_t max_radius;
            float    min_dist;
            uint32_t num_angle_steps;

            HoughCircleShapeDetectionParameters()
                : vote_min_threshold(default_circle_vote_min_threshold)
                , min_radius(default_min_radius)
                , max_radius(default_max_radius)
                , min_dist(default_min_dist)
                , num_angle_steps(default_num_angle_steps) {}

            HoughCircleShapeDetectionParameters(
                uint32_t vote_min_threshold,
                uint32_t min_radius,
                uint32_t max_radius,
                float    min_dist,
                uint32_t num_angle_steps
            )
                : vote_min_threshold(vote_min_threshold)
                , min_radius(min_radius)
                , max_radius(max_radius)
                , min_dist(min_dist)
                , num_angle_steps(num_angle_steps) {}
        };
    } // shape_detection
} // components

#endif // HOUGH_CIRCLE_SHAPE_DETECTION_PARAMETERS_H
