#ifndef DENOISING_COMPONENT_H
#define DENOISING_COMPONENT_H

#include "Component.h"
#include "Context.h"

namespace components {
    namespace denoising {
        class DenoisingComponent : public Component {
        public:
            DenoisingComponent();
            virtual ~DenoisingComponent() = default;

            void process(Context& context) override final;

        protected:
            virtual void applyDenoising() = 0;
        };
    } // denoising
} // components

#endif // DENOISING_COMPONENT_H
