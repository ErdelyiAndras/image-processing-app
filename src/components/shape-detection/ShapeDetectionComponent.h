#ifndef SHAPE_DETECTION_COMPONENT_H
#define SHAPE_DETECTION_COMPONENT_H

#include "Context.h"
#include "ParameterizedComponent.h"

namespace components {
    namespace shape_detection {
        template <typename T>
        class ShapeDetectionComponent : public ParameterizedComponent<T> {
        public:
            explicit ShapeDetectionComponent(
                const typename ParameterizedComponent<T>::ParamType& params
            ) : ParameterizedComponent<T>(params) {};

            virtual ~ShapeDetectionComponent() = default;

            void process(Context& context) override final {
                ParameterizedComponent<T>::process(context);
                applyShapeDetection();
                context.getShapeMap() = this->outputImage;
            }

        protected:
            virtual void processContext(const Context& context) override {
                ParameterizedComponent<T>::processContext(context);
                this->inputImage = context.getEdgeMap();
            }

            virtual void applyShapeDetection() = 0;
        };
    } // shape_detection
} // components

#endif // SHAPE_DETECTION_COMPONENT_H
