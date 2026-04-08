#include "HoughLineShapeDetectionGPU.h"

#include "Context.h"
#include "HoughLineShapeDetectionComponent.h"
#include "Image.h"
#include "kernel_sources.h"

#include <CL/cl.h>
#include <CL/opencl.hpp>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace components::shape_detection {
    HoughLineShapeDetectionGPU::HoughLineShapeDetectionGPU()
        : HoughLineShapeDetectionGPU(ParamType{}) {}

    HoughLineShapeDetectionGPU::HoughLineShapeDetectionGPU(const ParamType& params)
        : HoughLineShapeDetectionComponent(params) {
        initOpenCL(HOUGH_LINE_SHAPE_DETECTION_KERNEL_SOURCE);
    }

    HoughLineShapeDetectionGPU::HoughLineShapeDetectionGPU(
        float rho_resolution,
        float theta_resolution,
        int   vote_min_threshold,
        int   min_line_length,
        int   max_line_gap
    )
        : HoughLineShapeDetectionGPU(
            ParamType{
                rho_resolution,
                theta_resolution,
                vote_min_threshold,
                min_line_length,
                max_line_gap
            }
        ) {}

    void HoughLineShapeDetectionGPU::applyHoughTransform() {
        cl::Kernel vote_kernel{ program, "hough_line_vote" };
        vote_kernel.setArg(0, edge_map_buffer);
        vote_kernel.setArg(1, accumulator_buffer);
        vote_kernel.setArg(2, cos_table_buffer);
        vote_kernel.setArg(3, sin_table_buffer);
        vote_kernel.setArg(4, static_cast<int>(height));
        vote_kernel.setArg(5, static_cast<int>(width));
        vote_kernel.setArg(6, rho_max);
        vote_kernel.setArg(7, parameters.rho_resolution);
        vote_kernel.setArg(8, static_cast<int>(num_rho_bins));
        vote_kernel.setArg(9, static_cast<int>(num_theta_bins));

        queue.enqueueNDRangeKernel(vote_kernel, cl::NullRange, img_size, cl::NullRange);

        queue.enqueueReadBuffer(accumulator_buffer, CL_TRUE, 0, static_cast<size_t>(num_rho_bins) * num_theta_bins * sizeof(uint32_t), accumulator.data());
    }

    void HoughLineShapeDetectionGPU::processContext(const Context& context) {
        HoughLineShapeDetectionComponent::processContext(context);
        img_size = height * width;

        edge_map_buffer    = cl::Buffer{ cl_context, CL_MEM_READ_ONLY,  img_size * sizeof(float) };
        accumulator_buffer = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, static_cast<size_t>(num_rho_bins) * num_theta_bins * sizeof(uint32_t) };
        cos_table_buffer   = cl::Buffer{ cl_context, CL_MEM_READ_ONLY,  num_theta_bins * sizeof(float) };
        sin_table_buffer   = cl::Buffer{ cl_context, CL_MEM_READ_ONLY,  num_theta_bins * sizeof(float) };

        queue.enqueueWriteBuffer(edge_map_buffer,  CL_FALSE, 0, img_size * sizeof(float),       inputImage.data());
        queue.enqueueFillBuffer(accumulator_buffer, 0U, 0, static_cast<size_t>(num_rho_bins) * num_theta_bins * sizeof(uint32_t));
        queue.enqueueWriteBuffer(cos_table_buffer, CL_FALSE, 0, num_theta_bins * sizeof(float), cos_table.data());
        queue.enqueueWriteBuffer(sin_table_buffer, CL_FALSE, 0, num_theta_bins * sizeof(float), sin_table.data());
    }
} // namespace components::shape_detection
