#include "GaussianBlurComponent.h"
#include "GaussianBlurParameters.h"
#include "DenoisingComponent.h"
#include "Parameters.h"

#include <vector>
#include <cmath>
#include <typeinfo>

namespace components {
    namespace denoising {
        GaussianBlurComponent::GaussianBlurComponent(const GaussianBlurParameters& params)
            : DenoisingComponent()
            , kernel_size(params.kernel_size)
            , sigma(params.sigma) {}

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
