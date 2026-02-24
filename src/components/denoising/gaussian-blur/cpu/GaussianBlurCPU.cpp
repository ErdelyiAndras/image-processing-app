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

        void GaussianBlurCPU::applyGaussianBlur() {
            const int half{ kernel_size / 2 };

            std::vector<float> kernel(kernel_size * kernel_size);
            float sum{ 0.0f };
            for (int i{ -half }; i <= half; ++i) {
                for (int j{ -half }; j <= half; ++j) {
                    float val{
                        std::exp(-(static_cast<float>(i * i + j * j)) /
                        (2.0f * sigma * sigma))
                    };
                    kernel[(i + half) * kernel_size + (j + half)] = val;
                    sum += val;
                }
            }
            for (float& v : kernel) {
                v /= sum;
            }

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
