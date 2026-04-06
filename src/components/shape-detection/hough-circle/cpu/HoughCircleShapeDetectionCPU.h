#ifndef HOUGH_CIRCLE_SHAPE_DETECTION_CPU_H
#define HOUGH_CIRCLE_SHAPE_DETECTION_CPU_H

#include "HoughCircleShapeDetectionComponent.h"

#include <cstdint>
#include <string>

namespace components {
    namespace shape_detection {
        class HoughCircleShapeDetectionCPU final : public HoughCircleShapeDetectionComponent {
        public:
            HoughCircleShapeDetectionCPU();
            explicit HoughCircleShapeDetectionCPU(const ParamType& params);
            explicit HoughCircleShapeDetectionCPU(
                uint32_t vote_min_threshold,
                uint32_t min_radius,
                uint32_t max_radius,
                float    min_dist,
                uint32_t num_angle_steps
            );

            inline std::string getName() const override { return "hough-circle-cpu"; }

        private:
            void applyHoughTransform() override;
        };
    } // shape_detection
} // components

#endif // HOUGH_CIRCLE_SHAPE_DETECTION_CPU_H
