#ifndef GAUSSIAN_BLUR_COMPONENT_H
#define GAUSSIAN_BLUR_COMPONENT_H

#include "denoising-config.h"
#include "DenoisingComponent.h"
#include "GaussianBlurParameters.h"

#include <vector>

namespace components {
    namespace denoising {
        class GaussianBlurComponent : public DenoisingComponent {
        public:
            GaussianBlurComponent();
            GaussianBlurComponent(int kernel_size, float sigma);

            virtual ~GaussianBlurComponent() = default;

            inline int getKernelSize() const { return kernel_size; }
            inline float getSigma() const { return sigma; }

            void setParameters(const Parameters& params) override;

        protected:
            int kernel_size;
            float sigma;

            virtual void computeConvolution(const std::vector<float>& kernel) = 0;

        private:
            using ParamType = GaussianBlurParameters;

            void applyDenoising() override;
        };
    } // denoising
} // components

#endif // GAUSSIAN_BLUR_COMPONENT_H
