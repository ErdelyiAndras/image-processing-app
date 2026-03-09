#include "HoughCircleShapeDetectionCPU.h"
#include "config.h"

#include <cmath>
#include <vector>

namespace components {
    namespace shape_detection {
        HoughCircleShapeDetectionCPU::HoughCircleShapeDetectionCPU()
            : HoughCircleShapeDetectionComponent()
            , accumulator()
            , cos_table()
            , sin_table() {}

        HoughCircleShapeDetectionCPU::HoughCircleShapeDetectionCPU(
            uint32_t vote_min_threshold,
            uint32_t min_radius,
            uint32_t max_radius,
            float    min_dist,
            uint32_t num_angle_steps
        )
            : HoughCircleShapeDetectionComponent(vote_min_threshold, min_radius, max_radius, min_dist, num_angle_steps)
            , accumulator()
            , cos_table()
            , sin_table() {}

        void HoughCircleShapeDetectionCPU::applyHoughTransform() {
            for (PixelIdx py{ 0U }; py < height; ++py) {
                for (PixelIdx px{ 0U }; px < width; ++px) {
                    if (inputImage(py, px) == 0.0f) {
                        continue;
                    }

                    for (PixelIdx r{ min_radius }; r <= max_radius; ++r) {
                        const uint32_t r_idx{ r - min_radius };

                        for (uint32_t a{ 0U }; a < num_angle_steps; ++a) {
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

            for (uint32_t r_idx{ 0U }; r_idx < num_radii; ++r_idx) {
                const uint32_t r_lo{ r_idx > 0U            ? r_idx - 1U : 0U };
                const uint32_t r_hi{ r_idx < num_radii - 1 ? r_idx + 1U : num_radii - 1U };

                for (PixelIdx cy{ 1U }; cy < height - 1; ++cy) {
                    for (PixelIdx cx{ 1U }; cx < width - 1; ++cx) {
                        const uint32_t votes{
                            accumulator[(r_idx * height + cy) * width + cx]
                        };

                        if (votes < vote_min_threshold) {
                            continue;
                        }

                        bool is_local_max{ true };
                        for (uint32_t nr{ r_lo }; nr <= r_hi && is_local_max; ++nr) {
                            for (int dcy{ -1 }; dcy <= 1 && is_local_max; ++dcy) {
                                for (int dcx{ -1 }; dcx <= 1 && is_local_max; ++dcx) {
                                    if (nr == r_idx && dcy == 0 && dcx == 0) {
                                        continue;
                                    }
                                    const uint32_t neighbour_idx{
                                        (nr * height + static_cast<uint32_t>(static_cast<int>(cy) + dcy)) * width +
                                        static_cast<uint32_t>(static_cast<int>(cx) + dcx)
                                    };
                                    const uint32_t neighbour{ accumulator[neighbour_idx] };

                                    if (neighbour > votes) {
                                        is_local_max = false;
                                    }
                                }
                            }
                        }

                        if (!is_local_max) {
                            continue;
                        }

                        detected_circles.emplace_back(
                            HoughCircle{ cx, cy, static_cast<PixelIdx>(r_idx + min_radius), votes}
                        );
                    }
                }
            }
        }

        void HoughCircleShapeDetectionCPU::processContext(const Context& context) {
            ShapeDetectionComponent::processContext(context);

            cos_table.resize(num_angle_steps);
            sin_table.resize(num_angle_steps);
            for (uint32_t angle_idx{ 0U }; angle_idx < num_angle_steps; ++angle_idx) {
                const float angle{ static_cast<float>(angle_idx) * 2.0f * pi / static_cast<float>(num_angle_steps) };
                cos_table[angle_idx] = std::cos(angle);
                sin_table[angle_idx] = std::sin(angle);
            }

            accumulator.assign(num_radii * height * width, 0U);
        }
    } // shape_detection
} // components
