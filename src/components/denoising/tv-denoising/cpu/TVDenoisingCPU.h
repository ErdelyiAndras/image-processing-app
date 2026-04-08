#ifndef TV_DENOISING_CPU_H
#define TV_DENOISING_CPU_H

#include "Image.h"
#include "TVDenoisingComponent.h"

#include <cstdint>
#include <string>

namespace components {
    namespace denoising {
        class TVDenoisingCPU final : public TVDenoisingComponent {
        public:
            TVDenoisingCPU();
            explicit TVDenoisingCPU(const ParamType& params);
            explicit TVDenoisingCPU(float strength, float step_size, float tolerance);

            inline std::string getName() const override { return "tv-cpu"; }

        private:
            Image tv_gradient;
            Image l2_gradient;
            Image gradient;
            Image momentum;

            float tvNormAndGrad() override;
            float l2NormAndGrad() override;
            float evalLossAndGrad() override;
            void  evalMomentumAndUpdateImage(const uint32_t counter) override;
            void  postProcessing() override;

            void processContext(const Context& context) override;
        };
    } // denoising
} // components

#endif // TV_DENOISING_CPU_H
