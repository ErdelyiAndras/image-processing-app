#ifndef HOUGH_LINE_SHAPE_DETECTION_CPU_H
#define HOUGH_LINE_SHAPE_DETECTION_CPU_H

#include "HoughLineShapeDetectionComponent.h"
#include "Context.h"
#include "types.h"

#include <vector>

namespace components {
    namespace shape_detection {
        class HoughLineShapeDetectionCPU final : public HoughLineShapeDetectionComponent {
        public:
            HoughLineShapeDetectionCPU();
            HoughLineShapeDetectionCPU(
                float rho_resolution,
                float theta_resolution,
                uint32_t vote_min_threshold,
                uint32_t min_line_length,
                uint32_t max_line_gap
            );

        private:
            std::vector<uint32_t> accumulator;

            std::vector<float> cos_table;
            std::vector<float> sin_table;

            void applyHoughTransform() override;

            void processContext(const Context& context) override;
        };
    } // shape_detection
} // components

#endif // HOUGH_LINE_SHAPE_DETECTION_CPU_H
