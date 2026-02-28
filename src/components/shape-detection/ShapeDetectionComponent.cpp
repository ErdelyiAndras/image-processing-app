#include "ShapeDetectionComponent.h"

namespace components {
    namespace shape_detection {
        ShapeDetectionComponent::ShapeDetectionComponent() : Component() {}

        void ShapeDetectionComponent::process(Context& context) {
            processContext(context);
            applyShapeDetection();
            context.getShapeMap() = outputImage;
        }

        void ShapeDetectionComponent::processContext(const Context& context) {
            Component::processContext(context);
            inputImage = context.getEdgeMap();
        }
    } // shape_detection
} // components
