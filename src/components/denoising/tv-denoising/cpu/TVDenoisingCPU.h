#ifndef TV_DENOISING_CPU_H
#define TV_DENOISING_CPU_H

#include "DenoisingComponent.h"

namespace components {
    namespace denoising {
        class TVDenoisingCPU : public DenoisingComponent {
        public:
            TVDenoisingCPU();
            TVDenoisingCPU(float strength, float step_size, float tolerance);

            float tvNormAndGrad() override;
            float l2NormAndGrad() override;
            float evalLossAndGrad() override;
            void  evalMomentumAndUpdateImage(const uint64_t counter) override;
        };
    } // denoising
} // components

#endif // TV_DENOISING_CPU_H
