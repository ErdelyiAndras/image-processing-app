#include "SobelEdgeDetectionComponent.h"
#include "EdgeDetectionComponent.h"

namespace components {
    namespace edge_detection {
        SobelEdgeDetectionComponent::SobelEdgeDetectionComponent(const ParamType& params)
            : EdgeDetectionComponent(params) {}

        void SobelEdgeDetectionComponent::applyEdgeDetection() {
            applySobelFilter();
        }
    } // edge_detection
} // components
