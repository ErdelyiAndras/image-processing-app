#include "DenoisingComponent.h"

namespace components {
    namespace denoising {
        DenoisingComponent::DenoisingComponent() : Component() {}

        void DenoisingComponent::process(Context& context) {
            Component::process(context);
            applyDenoising();
            context.getProcessedImage() = outputImage;
        }
    } // denoising
} // components
