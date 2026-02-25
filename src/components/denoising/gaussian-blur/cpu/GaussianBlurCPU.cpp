#include "GaussianBlurCPU.h"

#include "types.h"

#include <cmath>
#include <vector>
#include <algorithm>

namespace components {
    namespace denoising {
        GaussianBlurCPU::GaussianBlurCPU() : GaussianBlurComponent() {}

        GaussianBlurCPU::GaussianBlurCPU(int kernel_size, float sigma)
            : GaussianBlurComponent(kernel_size, sigma) {}

        void GaussianBlurCPU::computeConvolution(const std::vector<float>& kernel) {
            const int half{ kernel_size / 2 };

            for (PixelIdx i{ 0U }; i < height; ++i) {
                for (PixelIdx j{ 0U }; j < width; ++j) {
                    float val{ 0.0f };
                    for (int ki{ -half }; ki <= half; ++ki) {
                        for (int kj{ -half }; kj <= half; ++kj) {
                            int ni{ std::clamp(static_cast<int>(i) + ki, 0, static_cast<int>(height) - 1) };
                            int nj{ std::clamp(static_cast<int>(j) + kj, 0, static_cast<int>(width) - 1) };
                            val += inputImage(static_cast<PixelIdx>(ni), static_cast<PixelIdx>(nj))
                                * kernel[(ki + half) * kernel_size + (kj + half)];
                        }
                    }
                    outputImage(i, j) = val;
                }
            }
        }
    } // denoising
} // components
