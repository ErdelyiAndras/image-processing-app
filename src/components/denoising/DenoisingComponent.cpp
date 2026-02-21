#include "DenoisingComponent.h"
#include "denoising-config.h"

#include <iostream>
#include <cmath>

namespace components {
    namespace denoising {
        DenoisingComponent::DenoisingComponent()
            : strength(default_strength)
            , step_size(default_step_size)
            , tolerance(default_tolerance)
            , step(step_size / (strength + 1))
            , height(0), width(0)
            , startImage()
            , processedImage()
            , tv_gradient()
            , l2_gradient()
            , gradient()
            , momentum() {}

        DenoisingComponent::DenoisingComponent(float strength, float step_size, float tolerance)
            : strength(strength)
            , step_size(step_size)
            , tolerance(tolerance)
            , step(step_size / (strength + 1))
            , height(0)
            , width(0)
            , startImage()
            , processedImage()
            , tv_gradient()
            , l2_gradient()
            , gradient()
            , momentum() {}

        void DenoisingComponent::setParameters(float strength, float step_size, float tolerance) {
            this->strength  = strength;
            this->step_size = step_size;
            this->tolerance = tolerance;
            this->step      = step_size / (strength + 1);
        }

        void DenoisingComponent::process(Context& context) {
            processContext(context);

            float loss_smoothed{ 0.0f };

            uint64_t counter { 1U };
            while (true) {
                float loss = evalLossAndGrad();

                if (ENABLE_LOGGING) {
                    std::cout << "Iteration: " << counter << ", Loss: " << loss << std::endl;
                }

                loss_smoothed = loss_smoothed * loss_smoothing_beta + loss * (1.0f - loss_smoothing_beta);
                float loss_smoothed_debiased = loss_smoothed / (1.0f - static_cast<float>(std::pow(loss_smoothing_beta, counter)));
                if (counter > 1 && loss_smoothed_debiased / loss < 1.0f + tolerance) {
                    if (ENABLE_LOGGING) {
                        std::cout << "Converged after " << counter << " iterations with loss: " << loss_smoothed_debiased << std::endl;
                    }
                    break;
                }

                evalMomentumAndUpdateImage(counter);

                ++counter;
            }

            context.getProcessedImage() = processedImage;
        }

        void DenoisingComponent::processContext(const Context& context) {
            startImage     = context.getProcessedImage();
            processedImage = startImage;

            height         = startImage.getRows();
            width          = startImage.getCols();

            tv_gradient = Image(height, width);
            l2_gradient = Image(height, width);
            gradient    = Image(height, width);
            momentum    = Image(height, width);
        }
    } // denoising
} // components
