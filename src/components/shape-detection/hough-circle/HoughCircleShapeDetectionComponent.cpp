#include "HoughCircleShapeDetectionComponent.h"
#include "HoughCircle.h"
#include "shape-detection-config.h"

#include <vector>
#include <algorithm>

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
            , detected_circles() {}

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
            , detected_circles() {}

        void HoughCircleShapeDetectionComponent::setParameters(const Parameters& params) {
            ParamType shapeDetectionParams{ dynamic_cast<const ParamType&>(params) };
            vote_min_threshold = shapeDetectionParams.vote_min_threshold;
            min_radius         = shapeDetectionParams.min_radius;
            max_radius         = shapeDetectionParams.max_radius;
            min_dist           = shapeDetectionParams.min_dist;
            num_angle_steps    = shapeDetectionParams.num_angle_steps;
            num_radii          = max_radius - min_radius + 1U;
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
                const PixelIdx cx{ circle.center_x };
                const PixelIdx cy{ circle.center_y };
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
        }

        void HoughCircleShapeDetectionComponent::applyShapeDetection() {
            applyHoughTransform();
            processDetectedCircles();
        }

        void HoughCircleShapeDetectionComponent::plot(int x, int y) {
            if (0 <= x && x < static_cast<int>(width) && 0 <= y && y < static_cast<int>(height)) {
                outputImage(static_cast<PixelIdx>(y), static_cast<PixelIdx>(x)) = 1.0f;
            }
        }
    } // shape_detection
} // components
