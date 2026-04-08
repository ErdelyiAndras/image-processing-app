#include "TVDenoisingComponent.h"

#include "Context.h"
#include "DenoisingComponent.h"
#include "Image.h"
#include "Parameters.h"
#include "config.h"

#include <cmath>
#include <cstdint>
#include <iostream>

namespace components::denoising {
    TVDenoisingComponent::TVDenoisingComponent(const ParamType& params)
        : DenoisingComponent(params)
        , step(params.step_size / (params.strength + 1)) {}

    void TVDenoisingComponent::setParameters(const Parameters& params) {
        DenoisingComponent::setParameters(params);
        step = parameters.step_size / (parameters.strength + 1);
    }

    void TVDenoisingComponent::processContext(const Context& context) {
        DenoisingComponent::processContext(context);
        outputImage = inputImage;
    }

    void TVDenoisingComponent::applyDenoising() {
        float loss_smoothed{ 0.0f };

        uint32_t counter { 1U };
        while (true) {
            const float loss{ evalLossAndGrad() };

            if (ENABLE_LOGGING) {
                std::cout << "Iteration: " << counter << ", Loss: " << loss << "\n";
            }

            loss_smoothed = loss_smoothed * loss_smoothing_beta + loss * (1.0f - loss_smoothing_beta);
            const float loss_smoothed_debiased{ loss_smoothed / (1.0f - std::pow(loss_smoothing_beta, static_cast<float>(counter))) };
            if (counter > 1 && std::abs(loss) > epsilon && loss_smoothed_debiased / loss < 1.0f + parameters.tolerance) {
                if (ENABLE_LOGGING) {
                    std::cout << "Converged after " << counter << " iterations with loss: " << loss_smoothed_debiased << "\n";
                }
                break;
            }

            evalMomentumAndUpdateImage(counter);

            ++counter;
        }

        postProcessing();
    }
} // namespace components::denoising
