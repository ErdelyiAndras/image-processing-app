#include "CannyEdgeDetectionComponent.h"
#include "EdgeDetectionComponent.h"

namespace components {
    namespace edge_detection {
        CannyEdgeDetectionComponent::CannyEdgeDetectionComponent(const ParamType& params)
            : EdgeDetectionComponent(params)
            , changed(true) {}

        void CannyEdgeDetectionComponent::processContext(const Context& context) {
            EdgeDetectionComponent::processContext(context);

            changed = true;
        }

        void CannyEdgeDetectionComponent::applyEdgeDetection() {
            calculateSobelGradient();
            nonMaximumSuppression();
            doubleThresholding();
            edgeTrackingByHysteresis();
        }
    } // edge_detection
} // components
