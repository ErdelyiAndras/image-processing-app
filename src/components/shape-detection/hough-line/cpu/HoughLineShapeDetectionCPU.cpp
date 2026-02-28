#include "HoughLineShapeDetectionCPU.h"
#include "types.h"

#include <cmath>
#include <algorithm>

namespace components {
    namespace shape_detection {
        HoughLineShapeDetectionCPU::HoughLineShapeDetectionCPU()
            : HoughLineShapeDetectionComponent()
            , accumulator()
            , cos_table()
            , sin_table() {}

        HoughLineShapeDetectionCPU::HoughLineShapeDetectionCPU(
            float rho_resolution,
            float theta_resolution,
            uint32_t vote_min_threshold,
            uint32_t min_line_length,
            uint32_t max_line_gap
        )
            : HoughLineShapeDetectionComponent(rho_resolution, theta_resolution, vote_min_threshold, min_line_length, max_line_gap)
            , accumulator()
            , cos_table()
            , sin_table() {}

        void HoughLineShapeDetectionCPU::applyHoughTransform() {
            for (PixelIdx i{ 0U }; i < height; ++i) {
                for (PixelIdx j{ 0U }; j < width; ++j) {
                    if (inputImage(i, j) == 0.0f) {
                        continue;
                    }

                    for (uint32_t theta_idx{ 0U }; theta_idx < num_theta_bins; ++theta_idx) {
                        const float rho{ static_cast<float>(j) * cos_table[theta_idx] + static_cast<float>(i) * sin_table[theta_idx] };
                        const uint32_t rho_idx{ static_cast<uint32_t>(std::round((rho + rho_max) / rho_resolution)) };
                        if (rho_idx >= 0 && rho_idx < num_rho_bins) {
                            const uint32_t acc_idx{ rho_idx * num_theta_bins + theta_idx };
                            ++accumulator[acc_idx];
                        }
                    }
                }
            }

            for (uint32_t rho_idx{ 1U }; rho_idx < num_rho_bins - 1; ++rho_idx) {
                for (uint32_t theta_idx{ 1U }; theta_idx < num_theta_bins - 1; ++theta_idx) {
                    const uint32_t acc_idx{ rho_idx * num_theta_bins + theta_idx };
                    const uint32_t votes{ accumulator[acc_idx] };
                    if (votes < vote_min_threshold) {
                        continue;
                    }

                    bool is_local_max = true;
                    for (int drho{ -1 }; drho <= 1 && is_local_max; ++drho) {
                        for (int dtheta{ -1 }; dtheta <= 1 && is_local_max; ++dtheta) {
                            if (drho == 0 && dtheta == 0) {
                                continue;
                            }
                            const uint32_t neighbor_idx{ (rho_idx + drho) * num_theta_bins + (theta_idx + dtheta) };
                            if (accumulator[neighbor_idx] > votes) {
                                is_local_max = false;
                            }
                        }
                    }

                    if (!is_local_max) {
                        continue;
                    }

                    const float rho{ static_cast<float>(rho_idx) * rho_resolution - rho_max };
                    const float theta{ static_cast<float>(theta_idx) * theta_resolution };
                    detected_lines.emplace_back(HoughLine{ rho, theta, votes});
                }
            }

            std::sort(detected_lines.begin(), detected_lines.end(),
                [](const HoughLine& a, const HoughLine& b) {
                    return a.votes > b.votes;
                }
            );
        }

        void HoughLineShapeDetectionCPU::processContext(const Context& context) {
            HoughLineShapeDetectionComponent::processContext(context);

            cos_table.resize(num_theta_bins);
            sin_table.resize(num_theta_bins);
            for (uint32_t theta_idx{ 0U }; theta_idx < num_theta_bins; ++theta_idx) {
                const float theta{ static_cast<float>(theta_idx) * theta_resolution };
                cos_table[theta_idx] = std::cos(theta);
                sin_table[theta_idx] = std::sin(theta);
            }
            accumulator.assign(num_rho_bins * num_theta_bins, 0U);
        }
    } // shape_detection
}