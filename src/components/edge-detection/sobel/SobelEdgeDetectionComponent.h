#ifndef SOBEL_EDGE_DETECTION_COMPONENT_H
#define SOBEL_EDGE_DETECTION_COMPONENT_H

#include "EdgeDetectionComponent.h"
#include "SobelEdgeDetectionParameters.h"

namespace components {
    namespace edge_detection {
        class SobelEdgeDetectionComponent : public EdgeDetectionComponent<SobelEdgeDetectionParameters> {
        public:
            explicit SobelEdgeDetectionComponent(const ParamType& params);

            virtual ~SobelEdgeDetectionComponent() = default;

            inline float getThreshold() const { return parameters.threshold; }

        protected:
            virtual void applySobelFilter() = 0;

        private:
            void applyEdgeDetection() override final;
        };
    } // edge_detection
} // components

#endif // SOBEL_EDGE_DETECTION_COMPONENT_H
