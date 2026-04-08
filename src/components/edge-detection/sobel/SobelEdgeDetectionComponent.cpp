#include "SobelEdgeDetectionComponent.h"

#include "EdgeDetectionComponent.h"

namespace components::edge_detection {
    SobelEdgeDetectionComponent::SobelEdgeDetectionComponent(const ParamType& params)
        : EdgeDetectionComponent(params) {}

    void SobelEdgeDetectionComponent::applyEdgeDetection() {
        applySobelFilter();
    }
} // namespace components::edge_detection
