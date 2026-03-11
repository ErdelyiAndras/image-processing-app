#include "HoughCircleShapeDetectionGPU.h"

namespace components {
    namespace shape_detection {
        HoughCircleShapeDetectionGPU::HoughCircleShapeDetectionGPU()
            : HoughCircleShapeDetectionComponent()
            , edge_map_buffer()
            , accumulator_buffer()
            , cos_table_buffer()
            , sin_table_buffer() {
            initOpenCL(HOUGH_CIRCLE_SHAPE_DETECTION_KERNEL_PATH);
        }

        HoughCircleShapeDetectionGPU::HoughCircleShapeDetectionGPU(
            uint32_t vote_min_threshold,
            uint32_t min_radius,
            uint32_t max_radius,
            float    min_dist,
            uint32_t num_angle_steps
        )
            : HoughCircleShapeDetectionComponent(vote_min_threshold, min_radius, max_radius, min_dist, num_angle_steps)
            , edge_map_buffer()
            , accumulator_buffer()
            , cos_table_buffer()
            , sin_table_buffer() {
            initOpenCL(HOUGH_CIRCLE_SHAPE_DETECTION_KERNEL_PATH);
        }

        void HoughCircleShapeDetectionGPU::applyHoughTransform() {
            cl::Kernel vote_kernel{ program, "hough_circle_vote" };
            vote_kernel.setArg(0, edge_map_buffer);
            vote_kernel.setArg(1, accumulator_buffer);
            vote_kernel.setArg(2, cos_table_buffer);
            vote_kernel.setArg(3, sin_table_buffer);
            vote_kernel.setArg(4, static_cast<int>(height));
            vote_kernel.setArg(5, static_cast<int>(width));
            vote_kernel.setArg(6, min_radius);
            vote_kernel.setArg(7, max_radius);
            vote_kernel.setArg(8, num_angle_steps);

            queue.enqueueNDRangeKernel(vote_kernel, cl::NullRange, img_size, cl::NullRange);

            queue.enqueueReadBuffer(accumulator_buffer, CL_TRUE, 0, num_radii * height * width * sizeof(uint32_t), accumulator.data());
        }

        void HoughCircleShapeDetectionGPU::processContext(const Context& context) {
            HoughCircleShapeDetectionComponent::processContext(context);
            img_size = height * width;

            edge_map_buffer    = cl::Buffer{ cl_context, CL_MEM_READ_ONLY, img_size * sizeof(float) };
            accumulator_buffer = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, num_radii * height * width * sizeof(uint32_t) };
            cos_table_buffer   = cl::Buffer{ cl_context, CL_MEM_READ_ONLY, num_angle_steps * sizeof(float) };
            sin_table_buffer   = cl::Buffer{ cl_context, CL_MEM_READ_ONLY, num_angle_steps * sizeof(float) };

            queue.enqueueWriteBuffer(edge_map_buffer, CL_FALSE, 0, img_size * sizeof(float), inputImage.data());
            queue.enqueueFillBuffer(accumulator_buffer, static_cast<uint32_t>(0U), 0, num_radii * height * width * sizeof(uint32_t));
            queue.enqueueWriteBuffer(cos_table_buffer, CL_FALSE, 0, num_angle_steps * sizeof(float), cos_table.data());
            queue.enqueueWriteBuffer(sin_table_buffer, CL_FALSE, 0, num_angle_steps * sizeof(float), sin_table.data());
        }
    } // shape_detection
} // components