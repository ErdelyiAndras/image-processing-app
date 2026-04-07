#ifndef HOUGH_LINE_SHAPE_DETECTION_PARAMETERS_H
#define HOUGH_LINE_SHAPE_DETECTION_PARAMETERS_H

#include "Parameters.h"
#include "shape-detection-config.h"
#include "types.h"

namespace components {
    namespace shape_detection {
        struct HoughLineShapeDetectionParameters : public Parameters {
            float rho_resolution;
            float theta_resolution;
            int   vote_min_threshold;
            int   min_line_length;
            int   max_line_gap;

            HoughLineShapeDetectionParameters()
                : rho_resolution(default_rho_resolution)
                , theta_resolution(default_theta_resolution)
                , vote_min_threshold(default_threshold)
                , min_line_length(default_min_line_length)
                , max_line_gap(default_max_line_gap) {}

            HoughLineShapeDetectionParameters(
                float rho_resolution,
                float theta_resolution,
                int   vote_min_threshold,
                int   min_line_length,
                int   max_line_gap
            )
                : rho_resolution(rho_resolution)
                , theta_resolution(theta_resolution)
                , vote_min_threshold(vote_min_threshold)
                , min_line_length(min_line_length)
                , max_line_gap(max_line_gap) {}
        };
    } // shape_detection
} // components

#endif // HOUGH_LINE_SHAPE_DETECTION_PARAMETERS_H
