#ifndef GAUSSIAN_BLUR_COMPONENT_H
#define GAUSSIAN_BLUR_COMPONENT_H

#include "DenoisingComponent.h"
#include "GaussianBlurParameters.h"

#include <vector>

namespace components {
    namespace denoising {
        class GaussianBlurComponent : public DenoisingComponent<GaussianBlurParameters> {
        public:
            explicit GaussianBlurComponent(const ParamType& params);

            virtual ~GaussianBlurComponent() = default;

            inline int   getKernelSize() const { return parameters.kernel_size; }
            inline float getSigma()      const { return parameters.sigma; }

        protected:
            virtual void computeConvolution(const std::vector<float>& kernel) = 0;

        private:
            void applyDenoising() override final;
        };
    } // denoising
} // components

#endif // GAUSSIAN_BLUR_COMPONENT_H
