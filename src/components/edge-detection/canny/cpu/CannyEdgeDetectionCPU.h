#ifndef CANNY_EDGE_DETECTION_CPU_H
#define CANNY_EDGE_DETECTION_CPU_H

#include "CannyEdgeDetectionComponent.h"
#include "Image.h"

namespace components {
    namespace edge_detection {
        class CannyEdgeDetectionCPU final : public CannyEdgeDetectionComponent {
        public:
            CannyEdgeDetectionCPU();
            CannyEdgeDetectionCPU(float low_threshold, float high_threshold);

        private:
            Image grad_mag;
            Image grad_dir;
            Image nms;

            void calculateSobelGradient() override;
            void nonMaximumSuppression() override;
            void doubleThresholding() override;
            void edgeTrackingByHysteresis() override;

            void processContext(const Context& context) override;
        };
    } // edge_detection
} // components

#endif // CANNY_EDGE_DETECTION_CPU_H
