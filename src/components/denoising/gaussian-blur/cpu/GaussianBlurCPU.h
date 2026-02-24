#ifndef GAUSSIAN_BLUR_CPU_H
#define GAUSSIAN_BLUR_CPU_H

#include "GaussianBlurComponent.h"

namespace components {
    namespace denoising {
        class GaussianBlurCPU : public GaussianBlurComponent {
        public:
            GaussianBlurCPU();
            GaussianBlurCPU(int kernel_size, float sigma);

        private:
            void applyGaussianBlur() override;
        };
    } // denoising
} // components

#endif // GAUSSIAN_BLUR_CPU_H
