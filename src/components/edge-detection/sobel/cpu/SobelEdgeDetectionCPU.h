#ifndef SOBEL_EDGE_DETECTION_CPU_H
#define SOBEL_EDGE_DETECTION_CPU_H

#include "SobelEdgeDetectionComponent.h"
#include "SobelEdgeDetectionParameters.h"

#include <string>

namespace components {
    namespace edge_detection {
        class SobelEdgeDetectionCPU final : public SobelEdgeDetectionComponent {
        public:
            SobelEdgeDetectionCPU();
            explicit SobelEdgeDetectionCPU(const SobelEdgeDetectionParameters& params);
            explicit SobelEdgeDetectionCPU(float threshold);

            inline std::string getName() const override { return "sobel-cpu"; }

        private:
            void applySobelFilter() override;
        };
    } // edge_detection
} // components

#endif // SOBEL_EDGE_DETECTION_CPU_H
