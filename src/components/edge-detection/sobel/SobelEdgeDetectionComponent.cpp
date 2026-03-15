#include "SobelEdgeDetectionComponent.h"
#include "edge-detection-config.h"
#include "Parameters.h"
#include "SobelEdgeDetectionParameters.h"

#include <typeinfo>

namespace components {
    namespace edge_detection {
        SobelEdgeDetectionComponent::SobelEdgeDetectionComponent()
            : threshold(default_threshold) {}

        SobelEdgeDetectionComponent::SobelEdgeDetectionComponent(float threshold)
            : threshold(threshold) {}

        void SobelEdgeDetectionComponent::setParameters(const Parameters& params) {
            const ParamType* edgeDetectionParams{ dynamic_cast<const ParamType*>(&params) };
            if (!edgeDetectionParams) {
                throw std::bad_cast{};
            }
            threshold = edgeDetectionParams->threshold;
        }

        void SobelEdgeDetectionComponent::applyEdgeDetection() {
            applySobelFilter();
        }
    } // edge_detection
} // components
