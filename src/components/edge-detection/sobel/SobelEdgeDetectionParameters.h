#ifndef SOBEL_EDGE_DETECTION_PARAMETERS_H
#define SOBEL_EDGE_DETECTION_PARAMETERS_H

#include "Parameters.h"

namespace components {
    namespace edge_detection {
        struct SobelEdgeDetectionParameters : public Parameters {
            float threshold;

            SobelEdgeDetectionParameters(float threshold)
                : threshold(threshold) {}
        };
    } // edge_detection
} // components

#endif // SOBEL_EDGE_DETECTION_PARAMETERS_H
