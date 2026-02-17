#include "TVDenoisingCPU.h"

#include "types.h"
#include "config.h"

#include <cmath>

namespace components {
    namespace denoising {
        TVDenoisingCPU::TVDenoisingCPU() : DenoisingComponent() {}

        TVDenoisingCPU::TVDenoisingCPU(float strength, float step_size, float tolerance)
            : DenoisingComponent(strength, step_size, tolerance) {}

        float TVDenoisingCPU::tvNormAndGrad() {
            float tv_norm{ 0.0f };
            tv_gradient.clear();
            for (PixelIdx i{ 0U }; i < height - 1; ++i) {
                for (PixelIdx j{ 0U }; j < width - 1; ++j) {
                    const float x_diff{ processedImage(i, j) - processedImage(i, j + 1) };
                    const float y_diff{ processedImage(i, j) - processedImage(i + 1, j) };
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
                    const float diff{ processedImage(i, j) - startImage(i, j) };
                    l2_gradient(i, j) = diff;
                    l2_norm += diff * diff;
                }
            }
            return 0.5f * l2_norm;
        }

        float TVDenoisingCPU::evalLossAndGrad() {
            const float tv_norm{ tvNormAndGrad() };
            for (PixelIdx i{ 0U }; i < height; ++i) {
                for (PixelIdx j{ 0U }; j < width; ++j) {
                    gradient(i, j) = strength * tv_gradient(i, j);
                }
            }

            const float l2_norm{ l2NormAndGrad() };
            for (PixelIdx i{ 0U }; i < height; ++i) {
                for (PixelIdx j{ 0U }; j < width; ++j) {
                    gradient(i, j) += l2_gradient(i, j);
                }
            }

            return strength * tv_norm + l2_norm;
        }

        void TVDenoisingCPU::evalMomentumAndUpdateImage(const uint64_t counter) {
            for (PixelIdx i = 0; i < height; ++i) {
                for (PixelIdx j = 0; j < width; ++j) {
                    momentum(i, j) *= momentum_beta;
                    momentum(i, j) += gradient(i, j) * (1.0f - momentum_beta);
                    processedImage(i, j) -= step / (1.0f - static_cast<float>(std::pow(momentum_beta, counter))) * momentum(i, j);
                }
            }
        }
    } // denoising
} // components
