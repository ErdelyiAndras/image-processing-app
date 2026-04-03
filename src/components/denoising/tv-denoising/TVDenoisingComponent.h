#ifndef TV_DENOISING_COMPONENT_H
#define TV_DENOISING_COMPONENT_H

#include "denoising-config.h"
#include "DenoisingComponent.h"
#include "TVDenoisingParameters.h"
#include "types.h"

namespace components {
    namespace denoising {
        class TVDenoisingComponent : public DenoisingComponent<TVDenoisingParameters> {
        public:
            explicit TVDenoisingComponent(const ParamType& params);

            virtual ~TVDenoisingComponent() = default;

            inline float getStrength()  const { return parameters.strength; }
            inline float getStepSize()  const { return parameters.step_size; }
            inline float getTolerance() const { return parameters.tolerance; }

            void setParameters(const Parameters& params) override final;

        protected:
            float step;
            static constexpr float momentum_beta       = components::denoising::momentum_beta;
            static constexpr float loss_smoothing_beta = components::denoising::loss_smoothing_beta;

            virtual float tvNormAndGrad() = 0;
            virtual float l2NormAndGrad() = 0;
            virtual float evalLossAndGrad() = 0;
            virtual void  evalMomentumAndUpdateImage(const uint32_t counter) = 0;
            virtual void  postProcessing() = 0;

            void processContext(const Context& context) override;

        private:
            void applyDenoising() override final;
        };
    } // denoising
} // components

#endif // TV_DENOISING_COMPONENT_H
