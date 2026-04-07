#ifndef HOUGH_LINE_SHAPE_DETECTION_CPU_H
#define HOUGH_LINE_SHAPE_DETECTION_CPU_H

#include "HoughLineShapeDetectionComponent.h"

#include <string>

namespace components {
    namespace shape_detection {
        class HoughLineShapeDetectionCPU final : public HoughLineShapeDetectionComponent {
        public:
            HoughLineShapeDetectionCPU();
            explicit HoughLineShapeDetectionCPU(const ParamType& params);
            explicit HoughLineShapeDetectionCPU(
                float rho_resolution,
                float theta_resolution,
                int   vote_min_threshold,
                int   min_line_length,
                int   max_line_gap
            );

            inline std::string getName() const override final { return "hough-line-cpu"; }

        private:
            void applyHoughTransform() override;
        };
    } // shape_detection
} // components

#endif // HOUGH_LINE_SHAPE_DETECTION_CPU_H
