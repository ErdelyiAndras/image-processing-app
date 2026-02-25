#ifndef SOBEL_EDGE_DETECTION_CPU_H
#define SOBEL_EDGE_DETECTION_CPU_H

#include "SobelEdgeDetectionComponent.h"

namespace components {
    namespace edge_detection {
        class SobelEdgeDetectionCPU : public SobelEdgeDetectionComponent {
        public:
            SobelEdgeDetectionCPU();
            SobelEdgeDetectionCPU(float threshold);

        private:
            void applySobelFilter() override final;
        };
    } // edge_detection
} // components

#endif // SOBEL_EDGE_DETECTION_CPU_H
