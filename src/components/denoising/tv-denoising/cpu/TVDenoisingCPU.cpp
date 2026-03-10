#include "TVDenoisingCPU.h"

#include "types.h"
#include "config.h"

#include <cmath>
#include <algorithm>

namespace components {
    namespace denoising {
        TVDenoisingCPU::TVDenoisingCPU()
            : TVDenoisingComponent()
            , tv_gradient()
            , l2_gradient()
            , gradient()
            , momentum() {}

        TVDenoisingCPU::TVDenoisingCPU(float strength, float step_size, float tolerance)
            : TVDenoisingComponent(strength, step_size, tolerance)
            , tv_gradient()
            , l2_gradient()
            , gradient()
            , momentum() {}

        float TVDenoisingCPU::tvNormAndGrad() {
            float tv_norm{ 0.0f };
            tv_gradient.clear();
            for (PixelIdx i{ 0U }; i < height - 1; ++i) {
                for (PixelIdx j{ 0U }; j < width - 1; ++j) {
                    const float x_diff{ outputImage(i, j) - outputImage(i, j + 1) };
                    const float y_diff{ outputImage(i, j) - outputImage(i + 1, j) };
                    const float grad_mag{ std::sqrt(x_diff * x_diff + y_diff * y_diff + epsilon) };
                    tv_norm += grad_mag;

                    const float dx{ x_diff / grad_mag };
                    const float dy{ y_diff / grad_mag };

                    tv_gradient(i, j) += dx + dy;
                    tv_gradient(i, j + 1) -= dx;
                    tv_gradient(i + 1, j) -= dy;
                }
            }

            return tv_norm;
        }

        float TVDenoisingCPU::l2NormAndGrad() {
            float l2_norm{ 0.0f };
            for (PixelIdx i{ 0U }; i < height; ++i) {
                for (PixelIdx j{ 0U }; j < width; ++j) {
                    const float diff{ outputImage(i, j) - inputImage(i, j) };
                    l2_gradient(i, j) = diff;
                    l2_norm += diff * diff;
                }
            }
            return 0.5f * l2_norm;
        }

        float TVDenoisingCPU::evalLossAndGrad() {
            const float tv_norm{ tvNormAndGrad() };
            const float l2_norm{ l2NormAndGrad() };

            for (PixelIdx i{ 0U }; i < height; ++i) {
                for (PixelIdx j{ 0U }; j < width; ++j) {
                    gradient(i, j) = strength * tv_gradient(i, j) + l2_gradient(i, j);
                }
            }

            return strength * tv_norm + l2_norm;
        }

        void TVDenoisingCPU::evalMomentumAndUpdateImage(const uint32_t counter) {
            for (PixelIdx i{ 0U }; i < height; ++i) {
                for (PixelIdx j{ 0U }; j < width; ++j) {
                    momentum(i, j) *= momentum_beta;
                    momentum(i, j) += gradient(i, j) * (1.0f - momentum_beta);
                    outputImage(i, j) -= step / (1.0f - static_cast<float>(std::pow(momentum_beta, static_cast<float>(counter)))) * momentum(i, j);
                }
            }
        }

        void TVDenoisingCPU::postProcessing() {
            // No post-processing needed for CPU implementation
        }

        void TVDenoisingCPU::processContext(const Context& context) {
            TVDenoisingComponent::processContext(context);

            tv_gradient = Image{ height, width };
            l2_gradient = Image{ height, width };
            gradient    = Image{ height, width };
            momentum    = Image{ height, width };
        }
    } // denoising
} // components
