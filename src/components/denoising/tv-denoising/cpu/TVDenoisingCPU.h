#ifndef TV_DENOISING_CPU_H
#define TV_DENOISING_CPU_H

#include "TVDenoisingComponent.h"
#include "types.h"

namespace components {
    namespace denoising {
        class TVDenoisingCPU final : public TVDenoisingComponent {
        public:
            TVDenoisingCPU();
            TVDenoisingCPU(float strength, float step_size, float tolerance);

        private:
            Image tv_gradient;
            Image l2_gradient;
            Image gradient;
            Image momentum;

            float tvNormAndGrad() override;
            float l2NormAndGrad() override;
            float evalLossAndGrad() override;
            void  evalMomentumAndUpdateImage(const uint64_t counter) override;
            void  postProcessing() override;

            void processContext(const Context& context) override;
        };
    } // denoising
} // components

#endif // TV_DENOISING_CPU_H
