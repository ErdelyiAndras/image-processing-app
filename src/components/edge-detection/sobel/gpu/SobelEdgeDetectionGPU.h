#ifndef SOBEL_EDGE_DETECTION_GPU_H
#define SOBEL_EDGE_DETECTION_GPU_H

#include "SobelEdgeDetectionComponent.h"
#include "GPUComponent.h"

#include <string>

namespace components {
    namespace edge_detection {
        class SobelEdgeDetectionGPU final : public SobelEdgeDetectionComponent, protected GPUComponent {
        public:
            SobelEdgeDetectionGPU();
            SobelEdgeDetectionGPU(float threshold);

            inline std::string getName() const override { return "sobel-gpu"; }

        private:
            void applySobelFilter() override;

            void processContext(const Context& context) override;
        };
    } // edge_detection
} // components

#endif // SOBEL_EDGE_DETECTION_GPU_H
