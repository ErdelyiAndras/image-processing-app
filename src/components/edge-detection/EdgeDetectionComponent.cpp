#include "EdgeDetectionComponent.h"

namespace components {
    namespace edge_detection {
        EdgeDetectionComponent::EdgeDetectionComponent() : Component() {}

        void EdgeDetectionComponent::process(Context& context) {
            processContext(context);
            applyEdgeDetection();
            context.getEdgeMap() = outputImage;
            context.getProcessedImage() = outputImage;
        }
    } // edge_detection
} // components
