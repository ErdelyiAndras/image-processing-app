#ifndef SOBEL_EDGE_DETECTION_CPU_H
#define SOBEL_EDGE_DETECTION_CPU_H

#include "SobelEdgeDetectionComponent.h"

#include <string>

namespace components {
    namespace edge_detection {
        class SobelEdgeDetectionCPU final : public SobelEdgeDetectionComponent {
        public:
            SobelEdgeDetectionCPU();
            explicit SobelEdgeDetectionCPU(const ParamType& params);
            explicit SobelEdgeDetectionCPU(float threshold);

            inline std::string getName() const override { return "sobel-cpu"; }

        private:
            void applySobelFilter() override;
        };
    } // edge_detection
} // components

#endif // SOBEL_EDGE_DETECTION_CPU_H
