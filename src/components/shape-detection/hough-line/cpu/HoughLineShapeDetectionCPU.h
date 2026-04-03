#ifndef HOUGH_LINE_SHAPE_DETECTION_CPU_H
#define HOUGH_LINE_SHAPE_DETECTION_CPU_H

#include "HoughLineShapeDetectionComponent.h"
#include "Context.h"
#include "types.h"

#include <vector>
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
                uint32_t vote_min_threshold,
                uint32_t min_line_length,
                uint32_t max_line_gap
            );

            inline std::string getName() const override final { return "hough-line-cpu"; }

        private:
            void applyHoughTransform() override;
        };
    } // shape_detection
} // components

#endif // HOUGH_LINE_SHAPE_DETECTION_CPU_H
