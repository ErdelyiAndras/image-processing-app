#ifndef DENOISING_COMPONENT_H
#define DENOISING_COMPONENT_H

#include "Context.h"
#include "ParameterizedComponent.h"

namespace components {
    namespace denoising {
        template <typename T>
        class DenoisingComponent : public ParameterizedComponent<T> {
        public:
            explicit DenoisingComponent(
                const typename ParameterizedComponent<T>::ParamType& params
            ) : ParameterizedComponent<T>(params) {};

            virtual ~DenoisingComponent() = default;

            void process(Context& context) override final {
                ParameterizedComponent<T>::process(context);
                applyDenoising();
                context.getProcessedImage() = this->outputImage;
            }

        protected:
            virtual void applyDenoising() = 0;
        };
    } // denoising
} // components

#endif // DENOISING_COMPONENT_H
