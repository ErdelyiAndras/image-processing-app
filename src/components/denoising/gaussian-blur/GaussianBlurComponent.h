#ifndef GAUSSIAN_BLUR_COMPONENT_H
#define GAUSSIAN_BLUR_COMPONENT_H

#include "denoising-config.h"
#include "DenoisingComponent.h"
#include "GaussianBlurParameters.h"

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
            void process(Context& context) override;

        protected:
            int kernel_size;
            float sigma;

            virtual void applyGaussianBlur() = 0;

        private:
            using ParamType = GaussianBlurParameters;
        };
    } // denoising
} // components

#endif // GAUSSIAN_BLUR_COMPONENT_H
