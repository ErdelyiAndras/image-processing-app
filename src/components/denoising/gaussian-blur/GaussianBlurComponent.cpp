#include "GaussianBlurComponent.h"
#include "GaussianBlurParameters.h"
#include "DenoisingComponent.h"
#include "Parameters.h"
#include "denoising-config.h"

#include <vector>
#include <cmath>
#include <typeinfo>

namespace components {
    namespace denoising {
        GaussianBlurComponent::GaussianBlurComponent()
            : DenoisingComponent()
            , kernel_size(default_kernel_size)
            , sigma(default_sigma) {}

        GaussianBlurComponent::GaussianBlurComponent(int kernel_size, float sigma)
            : DenoisingComponent()
            , kernel_size(kernel_size)
            , sigma(sigma) {}

        void GaussianBlurComponent::setParameters(const Parameters& params) {
            const ParamType* gaussianBlurParams{ dynamic_cast<const ParamType*>(&params) };
            if (!gaussianBlurParams) {
                throw std::bad_cast{};
            }
            kernel_size = gaussianBlurParams->kernel_size;
            sigma       = gaussianBlurParams->sigma;
        }

        void GaussianBlurComponent::applyDenoising() {
            const int half{ kernel_size / 2 };

            std::vector<float> kernel(static_cast<size_t>(kernel_size) * static_cast<size_t>(kernel_size));
            float sum{ 0.0f };
            for (int i{ -half }; i <= half; ++i) {
                for (int j{ -half }; j <= half; ++j) {
                    float val{
                        std::exp(-(static_cast<float>(i * i + j * j)) /
                        (2.0f * sigma * sigma))
                    };
                    kernel[static_cast<size_t>((i + half) * kernel_size + (j + half))] = val;
                    sum += val;
                }
            }
            for (float& v : kernel) {
                v /= sum;
            }

            computeConvolution(kernel);
        }
    } // denoising
} // components
