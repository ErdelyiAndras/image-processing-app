#ifndef EDGE_DETECTION_COMPONENT_H
#define EDGE_DETECTION_COMPONENT_H

#include "Context.h"
#include "ParameterizedComponent.h"

namespace components {
    namespace edge_detection {
        template <typename T>
        class EdgeDetectionComponent : public ParameterizedComponent<T> {
        public:
            explicit EdgeDetectionComponent(
                const typename ParameterizedComponent<T>::ParamType& params
            ) : ParameterizedComponent<T>(params) {};

            virtual ~EdgeDetectionComponent() = default;

            void process(Context& context) override final {
                ParameterizedComponent<T>::process(context);
                applyEdgeDetection();
                context.getEdgeMap() = this->outputImage;
            }

        protected:
            virtual void applyEdgeDetection() = 0;
        };
    } // edge_detection
} // components

#endif // EDGE_DETECTION_COMPONENT_H
