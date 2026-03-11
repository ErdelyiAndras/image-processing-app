#ifndef HOUGH_CIRCLE_SHAPE_DETECTION_GPU_H
#define HOUGH_CIRCLE_SHAPE_DETECTION_GPU_H

#include "HoughCircleShapeDetectionComponent.h"
#include "GPUComponent.h"
#include "Context.h"
#include "types.h"

#include <vector>
#include <CL/opencl.hpp>

namespace components {
    namespace shape_detection {
        class HoughCircleShapeDetectionGPU final : public HoughCircleShapeDetectionComponent, protected GPUComponent {
        public:
            HoughCircleShapeDetectionGPU();
            HoughCircleShapeDetectionGPU(
                uint32_t vote_min_threshold,
                uint32_t min_radius,
                uint32_t max_radius,
                float    min_dist,
                uint32_t num_angle_steps
            );

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

#endif // HOUGH_CIRCLE_SHAPE_DETECTION_GPU_H
