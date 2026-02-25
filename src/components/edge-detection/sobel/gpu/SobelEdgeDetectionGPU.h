#ifndef SOBEL_EDGE_DETECTION_GPU_H
#define SOBEL_EDGE_DETECTION_GPU_H

#include "SobelEdgeDetectionComponent.h"
#include "GPUComponent.h"

namespace components {
    namespace edge_detection {
        class SobelEdgeDetectionGPU : public SobelEdgeDetectionComponent, protected GPUComponent {
        public:
            SobelEdgeDetectionGPU();
            SobelEdgeDetectionGPU(float threshold);

        private:
            void applySobelFilter() override final;

            void processContext(const Context& context) override final;
        };
    } // edge_detection
} // components

#endif // SOBEL_EDGE_DETECTION_GPU_H
