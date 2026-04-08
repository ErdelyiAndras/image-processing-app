#include "HoughCircleShapeDetectionComponent.h"

#include "Context.h"
#include "HoughCircle.h"
#include "Image.h"
#include "Parameters.h"
#include "ShapeDetectionComponent.h"
#include "config.h"
#include "types.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace components::shape_detection {
    HoughCircleShapeDetectionComponent::HoughCircleShapeDetectionComponent(const ParamType& params)
        : ShapeDetectionComponent(params)
        , num_radii(static_cast<uint32_t>(params.max_radius - params.min_radius) + 1U) {}

    void HoughCircleShapeDetectionComponent::setParameters(const Parameters& params) {
        ShapeDetectionComponent::setParameters(params);
        num_radii = static_cast<uint32_t>(parameters.max_radius - parameters.min_radius) + 1U;
    }

    void HoughCircleShapeDetectionComponent::nonMaximumSuppression() {
        for (uint32_t r_idx{ 0U }; r_idx < num_radii; ++r_idx) {
            for (PixelIdx cy{ 1U }; cy < height - 1; ++cy) {
                for (PixelIdx cx{ 1U }; cx < width - 1; ++cx) {
                    const uint32_t votes{
                        accumulator[(((r_idx * height) + cy) * width) + cx]
                    };

                    if (votes < static_cast<uint32_t>(parameters.vote_min_threshold)) {
                        continue;
                    }

                    if (!isLocalMax(r_idx, cx, cy, votes)) {
                        continue;
                    }

                    detected_circles.emplace_back(
                        HoughCircle{ cx, cy, r_idx + static_cast<PixelIdx>(parameters.min_radius), votes}
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
        for (const HoughCircle& candidate : detected_circles) {
            if (isCircleTooClose(candidate, valid_circles)) {
                continue;
            }
            drawCircle(candidate);
            valid_circles.push_back(candidate);
        }

        detected_circles = std::move(valid_circles);
    }

    void HoughCircleShapeDetectionComponent::processContext(const Context& context) {
        ShapeDetectionComponent::processContext(context);
        detected_circles.clear();
        accumulator.assign(static_cast<size_t>(num_radii) * height * width, 0U);

        cos_table.resize(static_cast<size_t>(parameters.num_angle_steps));
        sin_table.resize(static_cast<size_t>(parameters.num_angle_steps));
        for (uint32_t angle_idx{ 0U }; angle_idx < static_cast<uint32_t>(parameters.num_angle_steps); ++angle_idx) {
            const float angle{ static_cast<float>(angle_idx) * 2.0f * pi / static_cast<float>(parameters.num_angle_steps) };
            cos_table[angle_idx] = std::cos(angle);
            sin_table[angle_idx] = std::sin(angle);
        }
    }

    void HoughCircleShapeDetectionComponent::applyShapeDetection() {
        applyHoughTransform();
        nonMaximumSuppression();
        processDetectedCircles();
    }

    bool HoughCircleShapeDetectionComponent::isCircleTooClose(const HoughCircle& circle, const std::vector<HoughCircle>& valid_circles) const {
        const float min_dist_sq{ parameters.min_dist * parameters.min_dist };
        return std::any_of(valid_circles.begin(), valid_circles.end(),
            [&circle, &min_dist_sq](const HoughCircle& accepted) {
                const float dcx{
                    static_cast<float>(circle.center_x) -
                    static_cast<float>(accepted.center_x)
                };
                const float dcy{
                    static_cast<float>(circle.center_y) -
                    static_cast<float>(accepted.center_y)
                };
                return dcx * dcx + dcy * dcy < min_dist_sq;
            }
        );
    }

    void HoughCircleShapeDetectionComponent::drawCircle(const HoughCircle& circle) {
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

    bool HoughCircleShapeDetectionComponent::isLocalMax(uint32_t r_idx, PixelIdx cx, PixelIdx cy, uint32_t votes) const {
        const uint32_t r_lo{ r_idx > 0U            ? r_idx - 1U : 0U };
        const uint32_t r_hi{ r_idx < num_radii - 1 ? r_idx + 1U : num_radii - 1U };

        for (uint32_t nr{ r_lo }; nr <= r_hi; ++nr) {
            for (int dcy{ -1 }; dcy <= 1; ++dcy) {
                for (int dcx{ -1 }; dcx <= 1; ++dcx) {
                    if (nr == r_idx && dcy == 0 && dcx == 0) {
                        continue;
                    }
                    const uint32_t neighbour_idx{
                        (((nr * height) + static_cast<uint32_t>(static_cast<int>(cy) + dcy)) * width) +
                        static_cast<uint32_t>(static_cast<int>(cx) + dcx)
                    };
                    const uint32_t neighbour{ accumulator[neighbour_idx] };

                    if (neighbour > votes) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    void HoughCircleShapeDetectionComponent::plot(int x, int y) {
        if (0 <= x && x < static_cast<int>(width) && 0 <= y && y < static_cast<int>(height)) {
            outputImage(static_cast<PixelIdx>(y), static_cast<PixelIdx>(x)) = 1.0f;
        }
    }
} // namespace components::shape_detection
