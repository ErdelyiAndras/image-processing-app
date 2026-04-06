#ifndef HOUGH_LINE_SHAPE_DETECTION_GPU_H
#define HOUGH_LINE_SHAPE_DETECTION_GPU_H

#include "HoughLineShapeDetectionComponent.h"
#include "GPUComponent.h"

#include <CL/opencl.hpp>
#include <cstdint>
#include <string>

namespace components {
    namespace shape_detection {
        class HoughLineShapeDetectionGPU final : public HoughLineShapeDetectionComponent, protected GPUComponent {
        public:
            HoughLineShapeDetectionGPU();
            explicit HoughLineShapeDetectionGPU(const ParamType& params);
            explicit HoughLineShapeDetectionGPU(
                float rho_resolution,
                float theta_resolution,
                uint32_t vote_min_threshold,
                uint32_t min_line_length,
                uint32_t max_line_gap
            );

            inline std::string getName() const override final { return "hough-line-gpu"; }

        private:
            cl::Buffer edge_map_buffer;
            cl::Buffer accumulator_buffer;
            cl::Buffer cos_table_buffer;
            cl::Buffer sin_table_buffer;

            void applyHoughTransform() override;

            void processContext(const Context& context) override;
        };
    } // shape_detection
} // components

#endif // HOUGH_LINE_SHAPE_DETECTION_GPU_H
