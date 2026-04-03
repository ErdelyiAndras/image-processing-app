#ifndef CANNY_EDGE_DETECTION_COMPONENT_H
#define CANNY_EDGE_DETECTION_COMPONENT_H

#include "edge-detection-config.h"
#include "EdgeDetectionComponent.h"
#include "CannyEdgeDetectionParameters.h"

namespace components {
    namespace edge_detection {
        class CannyEdgeDetectionComponent : public EdgeDetectionComponent<CannyEdgeDetectionParameters> {
        public:
            explicit CannyEdgeDetectionComponent(const ParamType& params);

            virtual ~CannyEdgeDetectionComponent() = default;

            inline float getLowThreshold()  const { return parameters.low_threshold; }
            inline float getHighThreshold() const { return parameters.high_threshold; }

        protected:
            bool changed;

            static constexpr float strong = components::edge_detection::strong;
            static constexpr float weak   = components::edge_detection::weak;

            virtual void calculateSobelGradient() = 0;
            virtual void nonMaximumSuppression() = 0;
            virtual void doubleThresholding() = 0;
            virtual void edgeTrackingByHysteresis() = 0;

            void processContext(const Context& context) override;

        private:
            void applyEdgeDetection() override final;
        };
    } // edge_detection
} // components

#endif // CANNY_EDGE_DETECTION_COMPONENT_H
