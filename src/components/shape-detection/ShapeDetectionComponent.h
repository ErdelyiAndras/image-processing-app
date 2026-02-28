#ifndef SHAPE_DETECTION_COMPONENT_H
#define SHAPE_DETECTION_COMPONENT_H

#include "Component.h"
#include "Context.h"

namespace components {
    namespace shape_detection {
        class ShapeDetectionComponent : public Component {
        public:
            ShapeDetectionComponent();
            virtual ~ShapeDetectionComponent() = default;

            void process(Context& context) override final;

        protected:
            virtual void processContext(const Context& context) override;

            virtual void applyShapeDetection() = 0;
        };
    } // shape_detection
} // components

#endif // SHAPE_DETECTION_COMPONENT_H
