#ifndef EDGE_DETECTION_COMPONENT_H
#define EDGE_DETECTION_COMPONENT_H

#include "Component.h"
#include "Context.h"

namespace components {
    namespace edge_detection {
        class EdgeDetectionComponent : public Component {
        public:
            EdgeDetectionComponent();
            virtual ~EdgeDetectionComponent() = default;

            void process(Context& context) override final;

        protected:
            virtual void applyEdgeDetection() = 0;
        };
    } // edge_detection
} // components

#endif // EDGE_DETECTION_COMPONENT_H
