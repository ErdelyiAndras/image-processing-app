#include "DenoisingComponent.h"

namespace components {
    namespace denoising {
        DenoisingComponent::DenoisingComponent() : Component() {}

        void DenoisingComponent::process(Context& context) {
            processContext(context);
            applyDenoising();
            context.getProcessedImage() = outputImage;
        }
    } // denoising
} // components
