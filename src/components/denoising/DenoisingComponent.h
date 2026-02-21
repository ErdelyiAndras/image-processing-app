#ifndef DENOISING_COMPONENT_H
#define DENOISING_COMPONENT_H

#include "denoising-config.h"
#include "types.h"
#include "Component.h"

namespace components {
    namespace denoising {
        class DenoisingComponent : public Component {
        public:
            DenoisingComponent();
            DenoisingComponent(float strength, float step_size, float tolerance);

            virtual ~DenoisingComponent() = default;

            inline float getStrength() const { return strength; }
            inline float getStepSize() const { return step_size; }
            inline float getTolerance() const { return tolerance; }

            void setParameters(float strength, float step_size, float tolerance);
            void process(Context& context) override;

        protected:
            float strength;
            float step_size;
            float tolerance;

            float step;
            static constexpr float momentum_beta       = components::denoising::momentum_beta;
            static constexpr float loss_smoothing_beta = components::denoising::loss_smoothing_beta;

            PixelIdx height;
            PixelIdx width;
            Image startImage;
            Image processedImage;

            Image tv_gradient;
            Image l2_gradient;
            Image gradient;
            Image momentum;

            virtual float tvNormAndGrad() = 0;
            virtual float l2NormAndGrad() = 0;
            virtual float evalLossAndGrad() = 0;
            virtual void  evalMomentumAndUpdateImage(const uint64_t counter) = 0;

            virtual void processContext(const Context& context);
        };
    } // denoising
} // components

#endif // DENOISING_COMPONENT_H
