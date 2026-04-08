#include "CannyEdgeDetectionComponent.h"

#include "Context.h"
#include "EdgeDetectionComponent.h"

namespace components::edge_detection {
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
} // namespace components::edge_detection
