#include "GaussianBlurComponent.h"

#include "DenoisingComponent.h"

#include <cmath>
#include <cstddef>
#include <vector>

namespace components::denoising {
    GaussianBlurComponent::GaussianBlurComponent(const ParamType& params)
        : DenoisingComponent(params) {}

    void GaussianBlurComponent::applyDenoising() {
        const int half{ parameters.kernel_size / 2 };

        std::vector<float> kernel(static_cast<size_t>(parameters.kernel_size) * static_cast<size_t>(parameters.kernel_size));
        float sum{ 0.0f };
        for (int i{ -half }; i <= half; ++i) {
            for (int j{ -half }; j <= half; ++j) {
                const float val{
                    std::exp(-(static_cast<float>((i * i) + (j * j))) /
                    (2.0f * parameters.sigma * parameters.sigma))
                };
                kernel[static_cast<size_t>((i + half) * parameters.kernel_size) + static_cast<size_t>(j + half)] = val;
                sum += val;
            }
        }
        for (float& v : kernel) {
            v /= sum;
        }

        computeConvolution(kernel);
    }
} // namespace components::denoising
