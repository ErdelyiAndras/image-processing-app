#include "TVDenoisingComponent.h"
#include "TVDenoisingParameters.h"
#include "DenoisingComponent.h"
#include "Parameters.h"
#include "types.h"
#include "config.h"

#include <iostream>
#include <cmath>
#include <typeinfo>

namespace components {
    namespace denoising {
        TVDenoisingComponent::TVDenoisingComponent(const TVDenoisingParameters& params)
            : DenoisingComponent()
            , strength(params.strength)
            , step_size(params.step_size)
            , tolerance(params.tolerance)
            , step(params.step_size / (params.strength + 1)) {}

        void TVDenoisingComponent::setParameters(const Parameters& params) {
            const ParamType* denoisingParams{ dynamic_cast<const ParamType*>(&params) };
            if (!denoisingParams) {
                throw std::bad_cast{};
            }
            strength  = denoisingParams->strength;
            step_size = denoisingParams->step_size;
            tolerance = denoisingParams->tolerance;
            step      = step_size / (strength + 1);
        }

        void TVDenoisingComponent::processContext(const Context& context) {
            DenoisingComponent::processContext(context);
            outputImage = inputImage;
        }

        void TVDenoisingComponent::applyDenoising() {
            float loss_smoothed{ 0.0f };

            uint32_t counter { 1U };
            while (true) {
                float loss{ evalLossAndGrad() };

                if (ENABLE_LOGGING) {
                    std::cout << "Iteration: " << counter << ", Loss: " << loss << std::endl;
                }

                loss_smoothed = loss_smoothed * loss_smoothing_beta + loss * (1.0f - loss_smoothing_beta);
                float loss_smoothed_debiased{ loss_smoothed / (1.0f - static_cast<float>(std::pow(loss_smoothing_beta, static_cast<float>(counter)))) };
                if (counter > 1 && std::abs(loss) > epsilon && loss_smoothed_debiased / loss < 1.0f + tolerance) {
                    if (ENABLE_LOGGING) {
                        std::cout << "Converged after " << counter << " iterations with loss: " << loss_smoothed_debiased << std::endl;
                    }
                    break;
                }

                evalMomentumAndUpdateImage(counter);

                ++counter;
            }

            postProcessing();
        }
    } // denoising
} // components
