#include "GaussianBlurComponent.h"
#include "GaussianBlurParameters.h"
#include "DenoisingComponent.h"
#include "Parameters.h"
#include "denoising-config.h"

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
            ParamType gaussianBlurParams{ dynamic_cast<const ParamType&>(params) };
            kernel_size = gaussianBlurParams.kernel_size;
            sigma       = gaussianBlurParams.sigma;
        }

        void GaussianBlurComponent::process(Context& context) {
            processContext(context);

            applyGaussianBlur();

            context.getProcessedImage() = outputImage;
        }
    } // denoising
} // components
