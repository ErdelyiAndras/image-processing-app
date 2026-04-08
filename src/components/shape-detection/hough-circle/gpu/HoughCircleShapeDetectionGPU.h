#ifndef HOUGH_CIRCLE_SHAPE_DETECTION_GPU_H
#define HOUGH_CIRCLE_SHAPE_DETECTION_GPU_H

#include "GPUComponent.h"
#include "HoughCircleShapeDetectionComponent.h"

#include <CL/opencl.hpp>
#include <string>

namespace components {
    namespace shape_detection {
        class HoughCircleShapeDetectionGPU final : public HoughCircleShapeDetectionComponent, protected GPUComponent {
        public:
            HoughCircleShapeDetectionGPU();
            explicit HoughCircleShapeDetectionGPU(const ParamType& params);
            explicit HoughCircleShapeDetectionGPU(
                int   vote_min_threshold,
                int   min_radius,
                int   max_radius,
                float min_dist,
                int   num_angle_steps
            );

            inline std::string getName() const override { return "hough-circle-gpu"; }

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
