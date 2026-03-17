#ifndef GAUSSIAN_BLUR_GPU_H
#define GAUSSIAN_BLUR_GPU_H

#include "GaussianBlurComponent.h"
#include "GPUComponent.h"

#include <vector>
#include <string>

namespace components {
    namespace denoising {
        class GaussianBlurGPU final : public GaussianBlurComponent, protected GPUComponent {
        public:
            GaussianBlurGPU();
            GaussianBlurGPU(int kernel_size, float sigma);

            inline std::string getName() const override { return "gauss-gpu"; }

        private:
            void computeConvolution(const std::vector<float>& kernel) override;

            void processContext(const Context& context) override;
        };
    } // denoising
} // components

#endif // GAUSSIAN_BLUR_GPU_H
