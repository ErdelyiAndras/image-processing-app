#include "CannyEdgeDetectionComponent.h"
#include "edge-detection-config.h"
#include "Parameters.h"
#include "CannyEdgeDetectionParameters.h"
#include "EdgeDetectionComponent.h"

#include <typeinfo>

namespace components {
    namespace edge_detection {
        CannyEdgeDetectionComponent::CannyEdgeDetectionComponent()
            : EdgeDetectionComponent()
            , low_threshold(default_low_threshold)
            , high_threshold(default_high_threshold)
            , changed(true) {}

        CannyEdgeDetectionComponent::CannyEdgeDetectionComponent(float low_threshold, float high_threshold)
            : EdgeDetectionComponent()
            , low_threshold(low_threshold)
            , high_threshold(high_threshold)
            , changed(true) {}

        void CannyEdgeDetectionComponent::setParameters(const Parameters& params) {
            const ParamType* cannyParams{ dynamic_cast<const ParamType*>(&params) };
            if (!cannyParams) {
                throw std::bad_cast{};
            }
            low_threshold  = cannyParams->low_threshold;
            high_threshold = cannyParams->high_threshold;
        }

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
