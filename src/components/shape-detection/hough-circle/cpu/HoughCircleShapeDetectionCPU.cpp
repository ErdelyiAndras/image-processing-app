#include "HoughCircleShapeDetectionCPU.h"

#include "HoughCircleShapeDetectionComponent.h"
#include "Image.h"
#include "types.h"

#include <cmath>
#include <cstdint>
#include <vector>

namespace components::shape_detection {
    HoughCircleShapeDetectionCPU::HoughCircleShapeDetectionCPU()
        : HoughCircleShapeDetectionCPU(ParamType{}) {}

    HoughCircleShapeDetectionCPU::HoughCircleShapeDetectionCPU(const ParamType& params)
        : HoughCircleShapeDetectionComponent(params) {}

    HoughCircleShapeDetectionCPU::HoughCircleShapeDetectionCPU(
        int   vote_min_threshold,
        int   min_radius,
        int   max_radius,
        float min_dist,
        int   num_angle_steps
    )
        : HoughCircleShapeDetectionCPU(
            ParamType{
                vote_min_threshold,
                min_radius,
                max_radius,
                min_dist,
                num_angle_steps
            }
        ) {}

    void HoughCircleShapeDetectionCPU::applyHoughTransform() {
        for (PixelIdx py{ 0U }; py < height; ++py) {
            for (PixelIdx px{ 0U }; px < width; ++px) {
                if (inputImage(py, px) == 0.0f) {
                    continue;
                }

                for (PixelIdx r{ static_cast<uint32_t>(parameters.min_radius) }; r <= static_cast<uint32_t>(parameters.max_radius); ++r) {
                    const uint32_t r_idx{ r - static_cast<uint32_t>(parameters.min_radius) };

                    for (uint32_t a{ 0U }; a < static_cast<uint32_t>(parameters.num_angle_steps); ++a) {
                        const int cx{
                            static_cast<int>(px) - static_cast<int>(std::round(static_cast<float>(r) * cos_table[a]))
                        };
                        const int cy{
                            static_cast<int>(py) - static_cast<int>(std::round(static_cast<float>(r) * sin_table[a]))
                        };

                        if (0 <= cx && cx < static_cast<int>(width) &&
                            0 <= cy && cy < static_cast<int>(height)) {
                            const uint32_t acc_idx{
                                (((r_idx * height) + static_cast<uint32_t>(cy)) * width) +
                                static_cast<uint32_t>(cx)
                            };
                            ++accumulator[acc_idx];
                        }
                    }
                }
            }
        }
    }
} // namespace components::shape_detection
