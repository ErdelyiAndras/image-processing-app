#include "HoughCircleShapeDetectionGPU.h"

#include "Context.h"
#include "HoughCircleShapeDetectionComponent.h"
#include "Image.h"
#include "kernel_sources.h"

#include <CL/cl.h>
#include <CL/opencl.hpp>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace components::shape_detection {
    HoughCircleShapeDetectionGPU::HoughCircleShapeDetectionGPU()
        : HoughCircleShapeDetectionGPU(ParamType{}) {}

    HoughCircleShapeDetectionGPU::HoughCircleShapeDetectionGPU(const ParamType& params)
        : HoughCircleShapeDetectionComponent(params) {
        initOpenCL(HOUGH_CIRCLE_SHAPE_DETECTION_KERNEL_SOURCE);
    }

    HoughCircleShapeDetectionGPU::HoughCircleShapeDetectionGPU(
        int   vote_min_threshold,
        int   min_radius,
        int   max_radius,
        float min_dist,
        int   num_angle_steps
    )
        : HoughCircleShapeDetectionGPU(
            ParamType{
                vote_min_threshold,
                min_radius,
                max_radius,
                min_dist,
                num_angle_steps
            }
        ) {}

    void HoughCircleShapeDetectionGPU::applyHoughTransform() {
        cl::Kernel vote_kernel{ program, "hough_circle_vote" };
        vote_kernel.setArg(0, edge_map_buffer);
        vote_kernel.setArg(1, accumulator_buffer);
        vote_kernel.setArg(2, cos_table_buffer);
        vote_kernel.setArg(3, sin_table_buffer);
        vote_kernel.setArg(4, static_cast<int>(height));
        vote_kernel.setArg(5, static_cast<int>(width));
        vote_kernel.setArg(6, parameters.min_radius);
        vote_kernel.setArg(7, parameters.max_radius);
        vote_kernel.setArg(8, parameters.num_angle_steps);

        queue.enqueueNDRangeKernel(vote_kernel, cl::NullRange, img_size, cl::NullRange);

        queue.enqueueReadBuffer(accumulator_buffer, CL_TRUE, 0, static_cast<size_t>(num_radii) * height * width * sizeof(uint32_t), accumulator.data());
    }

    void HoughCircleShapeDetectionGPU::processContext(const Context& context) {
        HoughCircleShapeDetectionComponent::processContext(context);
        img_size = height * width;

        edge_map_buffer    = cl::Buffer{ cl_context, CL_MEM_READ_ONLY,  img_size * sizeof(float) };
        accumulator_buffer = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, static_cast<size_t>(num_radii) * height * width * sizeof(uint32_t) };
        cos_table_buffer   = cl::Buffer{ cl_context, CL_MEM_READ_ONLY,  static_cast<size_t>(parameters.num_angle_steps) * sizeof(float) };
        sin_table_buffer   = cl::Buffer{ cl_context, CL_MEM_READ_ONLY,  static_cast<size_t>(parameters.num_angle_steps) * sizeof(float) };

        queue.enqueueWriteBuffer(edge_map_buffer,  CL_FALSE, 0, img_size * sizeof(float),        inputImage.data());
        queue.enqueueFillBuffer(accumulator_buffer, static_cast<uint32_t>(0U), 0, static_cast<size_t>(num_radii) * height * width * sizeof(uint32_t));
        queue.enqueueWriteBuffer(cos_table_buffer, CL_FALSE, 0, static_cast<size_t>(parameters.num_angle_steps) * sizeof(float), cos_table.data());
        queue.enqueueWriteBuffer(sin_table_buffer, CL_FALSE, 0, static_cast<size_t>(parameters.num_angle_steps) * sizeof(float), sin_table.data());
    }
} // namespace components::shape_detection
