#include "HoughCircleShapeDetectionComponent.h"
#include "HoughCircle.h"
#include "shape-detection-config.h"

#include <vector>
#include <algorithm>
#include <cmath>

namespace components {
    namespace shape_detection {
        HoughCircleShapeDetectionComponent::HoughCircleShapeDetectionComponent()
            : ShapeDetectionComponent()
            , vote_min_threshold(default_circle_vote_min_threshold)
            , min_radius(default_min_radius)
            , max_radius(default_max_radius)
            , min_dist(default_min_dist)
            , num_angle_steps(default_num_angle_steps)
            , num_radii(default_max_radius - default_min_radius + 1U)
            , detected_circles()
            , accumulator()
            , cos_table()
            , sin_table() {}

        HoughCircleShapeDetectionComponent::HoughCircleShapeDetectionComponent(
            uint32_t vote_min_threshold,
            uint32_t min_radius,
            uint32_t max_radius,
            float    min_dist,
            uint32_t num_angle_steps
        )
            : ShapeDetectionComponent()
            , vote_min_threshold(vote_min_threshold)
            , min_radius(min_radius)
            , max_radius(max_radius)
            , min_dist(min_dist)
            , num_angle_steps(num_angle_steps)
            , num_radii(max_radius - min_radius + 1U)
            , detected_circles()
            , accumulator()
            , cos_table()
            , sin_table() {}

        void HoughCircleShapeDetectionComponent::setParameters(const Parameters& params) {
            const ParamType& shapeDetectionParams{ dynamic_cast<const ParamType&>(params) };
            vote_min_threshold = shapeDetectionParams.vote_min_threshold;
            min_radius         = shapeDetectionParams.min_radius;
            max_radius         = shapeDetectionParams.max_radius;
            min_dist           = shapeDetectionParams.min_dist;
            num_angle_steps    = shapeDetectionParams.num_angle_steps;
            num_radii          = max_radius - min_radius + 1U;
        }

        void HoughCircleShapeDetectionComponent::nonMaximumSuppression() {
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

        void HoughCircleShapeDetectionComponent::processDetectedCircles() {
            std::sort(detected_circles.begin(), detected_circles.end(),
                [](const HoughCircle& a, const HoughCircle& b) {
                    return a.votes > b.votes;
                }
            );

            std::vector<HoughCircle> valid_circles;
            const float min_dist_sq{ min_dist * min_dist };

            for (const HoughCircle& candidate : detected_circles) {
                bool too_close{ false };
                for (const HoughCircle& accepted : valid_circles) {
                    const float dcx{
                        static_cast<float>(candidate.center_x) -
                        static_cast<float>(accepted.center_x)
                    };
                    const float dcy{
                        static_cast<float>(candidate.center_y) -
                        static_cast<float>(accepted.center_y)
                    };
                    if (dcx * dcx + dcy * dcy < min_dist_sq) {
                        too_close = true;
                        break;
                    }
                }
                if (!too_close) {
                    valid_circles.push_back(candidate);
                }
            }

            detected_circles = std::move(valid_circles);

            for (const HoughCircle& circle : detected_circles) {
                const int cx{ static_cast<int>(circle.center_x) };
                const int cy{ static_cast<int>(circle.center_y) };
                const PixelIdx r{ circle.radius };

                int x{ 0 };
                int y{ static_cast<int>(r) };
                int d{ 1 - static_cast<int>(r) };

                while (x <= y) {
                    plot(cx + x, cy + y);
                    plot(cx - x, cy + y);
                    plot(cx + x, cy - y);
                    plot(cx - x, cy - y);
                    plot(cx + y, cy + x);
                    plot(cx - y, cy + x);
                    plot(cx + y, cy - x);
                    plot(cx - y, cy - x);

                    if (d < 0) {
                        d += 2 * x + 3;
                    }
                    else {
                        d += 2 * (x - y) + 5;
                        --y;
                    }
                    ++x;
                }
            }
        }

        void HoughCircleShapeDetectionComponent::processContext(const Context& context) {
            ShapeDetectionComponent::processContext(context);
            detected_circles.clear();
            accumulator.assign(num_radii * height * width, 0U);

            cos_table.resize(num_angle_steps);
            sin_table.resize(num_angle_steps);
            for (uint32_t angle_idx{ 0U }; angle_idx < num_angle_steps; ++angle_idx) {
                const float angle{ static_cast<float>(angle_idx) * 2.0f * pi / static_cast<float>(num_angle_steps) };
                cos_table[angle_idx] = std::cos(angle);
                sin_table[angle_idx] = std::sin(angle);
            }
        }

        void HoughCircleShapeDetectionComponent::applyShapeDetection() {
            applyHoughTransform();
            nonMaximumSuppression();
            processDetectedCircles();
        }

        void HoughCircleShapeDetectionComponent::plot(int x, int y) {
            if (0 <= x && x < static_cast<int>(width) && 0 <= y && y < static_cast<int>(height)) {
                outputImage(static_cast<PixelIdx>(y), static_cast<PixelIdx>(x)) = 1.0f;
            }
        }
    } // shape_detection
} // components
