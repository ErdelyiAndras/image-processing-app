#ifndef GAUSSIAN_BLUR_CPU_H
#define GAUSSIAN_BLUR_CPU_H

#include "GaussianBlurComponent.h"

#include <vector>
#include <string>

namespace components {
    namespace denoising {
        class GaussianBlurCPU final : public GaussianBlurComponent {
        public:
            GaussianBlurCPU();
            explicit GaussianBlurCPU(const ParamType& params);
            explicit GaussianBlurCPU(int kernel_size, float sigma);

            inline std::string getName() const override { return "gauss-cpu"; }

        private:
            void computeConvolution(const std::vector<float>& kernel) override;
        };
    } // denoising
} // components

#endif // GAUSSIAN_BLUR_CPU_H
