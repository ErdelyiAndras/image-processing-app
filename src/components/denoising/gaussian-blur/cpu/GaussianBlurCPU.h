#ifndef GAUSSIAN_BLUR_CPU_H
#define GAUSSIAN_BLUR_CPU_H

#include "GaussianBlurComponent.h"

#include <vector>

namespace components {
    namespace denoising {
        class GaussianBlurCPU final : public GaussianBlurComponent {
        public:
            GaussianBlurCPU();
            GaussianBlurCPU(int kernel_size, float sigma);

        private:
            void computeConvolution(const std::vector<float>& kernel) override;
        };
    } // denoising
} // components

#endif // GAUSSIAN_BLUR_CPU_H
