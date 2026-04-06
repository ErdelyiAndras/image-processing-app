#include "HoughCircleShapeDetectionCPU.h"
#include "HoughCircleShapeDetectionParameters.h"
#include "Image.h"
#include "types.h"

#include <cmath>
#include <vector>

namespace components {
    namespace shape_detection {
        HoughCircleShapeDetectionCPU::HoughCircleShapeDetectionCPU()
            : HoughCircleShapeDetectionCPU(ParamType{}) {}

        HoughCircleShapeDetectionCPU::HoughCircleShapeDetectionCPU(const ParamType& params)
            : HoughCircleShapeDetectionComponent(params) {}

        HoughCircleShapeDetectionCPU::HoughCircleShapeDetectionCPU(
            uint32_t vote_min_threshold,
            uint32_t min_radius,
            uint32_t max_radius,
            float    min_dist,
            uint32_t num_angle_steps
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

                    for (PixelIdx r{ parameters.min_radius }; r <= parameters.max_radius; ++r) {
                        const uint32_t r_idx{ r - parameters.min_radius };

                        for (uint32_t a{ 0U }; a < parameters.num_angle_steps; ++a) {
                            const int cx{
                                static_cast<int>(px) - static_cast<int>(std::round(static_cast<float>(r) * cos_table[a]))
                            };
                            const int cy{
                                static_cast<int>(py) - static_cast<int>(std::round(static_cast<float>(r) * sin_table[a]))
                            };

                            if (0 <= cx && cx < static_cast<int>(width) &&
                                0 <= cy && cy < static_cast<int>(height)) {
                                const uint32_t acc_idx{ (r_idx * height + static_cast<uint32_t>(cy)) * width + static_cast<uint32_t>(cx) };
                                ++accumulator[acc_idx];
                            }
                        }
                    }
                }
            }
        }
    } // shape_detection
} // components
