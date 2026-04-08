#ifndef SOBEL_EDGE_DETECTION_GPU_H
#define SOBEL_EDGE_DETECTION_GPU_H

#include "GPUComponent.h"
#include "SobelEdgeDetectionComponent.h"

#include <string>

namespace components {
    namespace edge_detection {
        class SobelEdgeDetectionGPU final : public SobelEdgeDetectionComponent, protected GPUComponent {
        public:
            SobelEdgeDetectionGPU();
            explicit SobelEdgeDetectionGPU(const ParamType& params);
            explicit SobelEdgeDetectionGPU(float threshold);

            inline std::string getName() const override { return "sobel-gpu"; }

        private:
            void applySobelFilter() override;

            void processContext(const Context& context) override;
        };
    } // edge_detection
} // components

#endif // SOBEL_EDGE_DETECTION_GPU_H
