#ifndef DENOISING_COMPONENT_H
#define DENOISING_COMPONENT_H

#include "Component.h"

namespace components {
    namespace denoising {
        class DenoisingComponent : public Component {
        public:
            DenoisingComponent();
            virtual ~DenoisingComponent() = default;
        };
    } // denoising
} // components

#endif // DENOISING_COMPONENT_H
