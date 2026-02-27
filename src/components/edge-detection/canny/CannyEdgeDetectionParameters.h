#ifndef CANNY_EDGE_DETECTION_PARAMETERS_H
#define CANNY_EDGE_DETECTION_PARAMETERS_H

#include "Parameters.h"

namespace components {
    namespace edge_detection {
        struct CannyEdgeDetectionParameters : public Parameters {
            float low_threshold;
            float high_threshold;

            CannyEdgeDetectionParameters(float low_threshold, float high_threshold)
                : low_threshold(low_threshold), high_threshold(high_threshold) {}
        };
    }
}

#endif // CANNY_EDGE_DETECTION_PARAMETERS_H
