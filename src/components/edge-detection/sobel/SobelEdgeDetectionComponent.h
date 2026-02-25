#ifndef SOBEL_EDGE_DETECTION_COMPONENT_H
#define SOBEL_EDGE_DETECTION_COMPONENT_H

#include "edge-detection-config.h"
#include "EdgeDetectionComponent.h"
#include "SobelEdgeDetectionParameters.h"

namespace components {
    namespace edge_detection {
        class SobelEdgeDetectionComponent : public EdgeDetectionComponent {
        public:
            SobelEdgeDetectionComponent();
            SobelEdgeDetectionComponent(float threshold);

            virtual ~SobelEdgeDetectionComponent() = default;

            inline float getThreshold() const { return threshold; }

            void setParameters(const Parameters& params) override final;

        protected:
            float threshold;

            virtual void applySobelFilter() = 0;

        private:
            using ParamType = SobelEdgeDetectionParameters;

            void applyEdgeDetection() override final;
        };
    } // edge_detection
} // components

#endif // SOBEL_EDGE_DETECTION_COMPONENT_H
