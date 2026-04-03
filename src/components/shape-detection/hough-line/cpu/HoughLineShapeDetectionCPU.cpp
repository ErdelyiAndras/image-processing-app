#include "HoughLineShapeDetectionCPU.h"
#include "types.h"

#include <cmath>

namespace components {
    namespace shape_detection {
        HoughLineShapeDetectionCPU::HoughLineShapeDetectionCPU()
            : HoughLineShapeDetectionCPU(ParamType{}) {}

        HoughLineShapeDetectionCPU::HoughLineShapeDetectionCPU(const ParamType& params)
            : HoughLineShapeDetectionComponent(params) {}

        HoughLineShapeDetectionCPU::HoughLineShapeDetectionCPU(
            float rho_resolution,
            float theta_resolution,
            uint32_t vote_min_threshold,
            uint32_t min_line_length,
            uint32_t max_line_gap
        )
            : HoughLineShapeDetectionCPU(
                ParamType{
                    rho_resolution,
                    theta_resolution,
                    vote_min_threshold,
                    min_line_length,
                    max_line_gap
                }
            ) {}

        void HoughLineShapeDetectionCPU::applyHoughTransform() {
            for (PixelIdx i{ 0U }; i < height; ++i) {
                for (PixelIdx j{ 0U }; j < width; ++j) {
                    if (inputImage(i, j) == 0.0f) {
                        continue;
                    }

                    for (uint32_t theta_idx{ 0U }; theta_idx < num_theta_bins; ++theta_idx) {
                        const float rho{ static_cast<float>(j) * cos_table[theta_idx] + static_cast<float>(i) * sin_table[theta_idx] };
                        const float rho_idx_float{ std::round((rho + rho_max) / parameters.rho_resolution) };
                        if (0.0f <= rho_idx_float && rho_idx_float < static_cast<float>(num_rho_bins)) {
                            const uint32_t rho_idx{ static_cast<uint32_t>(rho_idx_float) };
                            const uint32_t acc_idx{ rho_idx * num_theta_bins + theta_idx };
                            ++accumulator[acc_idx];
                        }
                    }
                }
            }
        }
    } // shape_detection
} // components
