#ifndef HOUGH_CIRCLE_SHAPE_DETECTION_CPU_H
#define HOUGH_CIRCLE_SHAPE_DETECTION_CPU_H

#include "HoughCircleShapeDetectionComponent.h"

#include <string>

namespace components {
    namespace shape_detection {
        class HoughCircleShapeDetectionCPU final : public HoughCircleShapeDetectionComponent {
        public:
            HoughCircleShapeDetectionCPU();
            explicit HoughCircleShapeDetectionCPU(const ParamType& params);
            explicit HoughCircleShapeDetectionCPU(
                int   vote_min_threshold,
                int   min_radius,
                int   max_radius,
                float min_dist,
                int   num_angle_steps
            );

            inline std::string getName() const override { return "hough-circle-cpu"; }

        private:
            void applyHoughTransform() override;
        };
    } // shape_detection
} // components

#endif // HOUGH_CIRCLE_SHAPE_DETECTION_CPU_H
