#include "CannyEdgeDetectionGPU.h"
#include "kernel_sources.h"
#include "types.h"

#include <CL/opencl.h>

#include <iostream>
#include <utility>
#include <vector>

namespace components {
    namespace edge_detection {
        CannyEdgeDetectionGPU::CannyEdgeDetectionGPU()
            : CannyEdgeDetectionComponent()
            , grad_mag_buffer()
            , grad_dir_buffer()
            , nms_buffer()
            , output_buffer()
            , temp_out_buffer() {
            initOpenCL(CANNY_EDGE_DETECTION_KERNEL_SOURCE);
        }

        CannyEdgeDetectionGPU::CannyEdgeDetectionGPU(float low_threshold, float high_threshold)
            : CannyEdgeDetectionComponent(low_threshold, high_threshold)
            , grad_mag_buffer()
            , grad_dir_buffer()
            , nms_buffer()
            , output_buffer()
            , temp_out_buffer() {
            initOpenCL(CANNY_EDGE_DETECTION_KERNEL_SOURCE);
        }

        void CannyEdgeDetectionGPU::calculateSobelGradient() {
            cl::Buffer img_buf{ cl_context, CL_MEM_READ_ONLY, img_size * sizeof(float) };
            queue.enqueueWriteBuffer(img_buf, CL_FALSE, 0, img_size * sizeof(float), inputImage.data());

            queue.enqueueFillBuffer(grad_mag_buffer, 0.0f, 0, img_size * sizeof(float));
            queue.enqueueFillBuffer(grad_dir_buffer, 0.0f, 0, img_size * sizeof(float));

            cl::Kernel grad_kernel{ program, "canny_gradient" };
            grad_kernel.setArg(0, img_buf);
            grad_kernel.setArg(1, grad_mag_buffer);
            grad_kernel.setArg(2, grad_dir_buffer);
            grad_kernel.setArg(3, static_cast<int>(height));
            grad_kernel.setArg(4, static_cast<int>(width));

            queue.enqueueNDRangeKernel(grad_kernel, cl::NullRange, img_size, cl::NullRange);
        }

        void CannyEdgeDetectionGPU::nonMaximumSuppression() {
            queue.enqueueFillBuffer(nms_buffer, 0.0f, 0, img_size * sizeof(float));

            cl::Kernel nms_kernel{ program, "canny_nms" };
            nms_kernel.setArg(0, grad_mag_buffer);
            nms_kernel.setArg(1, grad_dir_buffer);
            nms_kernel.setArg(2, nms_buffer);
            nms_kernel.setArg(3, static_cast<int>(height));
            nms_kernel.setArg(4, static_cast<int>(width));

            queue.enqueueNDRangeKernel(nms_kernel, cl::NullRange, img_size, cl::NullRange);
        }

        void CannyEdgeDetectionGPU::doubleThresholding() {
            cl::Kernel thresh_kernel{ program, "canny_double_threshold" };
            thresh_kernel.setArg(0, nms_buffer);
            thresh_kernel.setArg(1, output_buffer);
            thresh_kernel.setArg(2, low_threshold);
            thresh_kernel.setArg(3, high_threshold);
            thresh_kernel.setArg(4, strong);
            thresh_kernel.setArg(5, weak);
            thresh_kernel.setArg(6, static_cast<int>(height));
            thresh_kernel.setArg(7, static_cast<int>(width));

            queue.enqueueNDRangeKernel(thresh_kernel, cl::NullRange, img_size, cl::NullRange);
        }

        void CannyEdgeDetectionGPU::edgeTrackingByHysteresis() {
            cl::Buffer changed_buf{ cl_context, CL_MEM_READ_WRITE, sizeof(int) };

            cl::Kernel hyst_kernel{ program, "canny_hysteresis" };
            hyst_kernel.setArg(2, changed_buf);
            hyst_kernel.setArg(3, strong);
            hyst_kernel.setArg(4, weak);
            hyst_kernel.setArg(5, static_cast<int>(height));
            hyst_kernel.setArg(6, static_cast<int>(width));

            cl::Buffer* read_buf{ &output_buffer };
            cl::Buffer* write_buf{ &temp_out_buffer };

            uint32_t counter{ 1U };
            while (changed) {
                int changed_int{ 0 };
                queue.enqueueWriteBuffer(changed_buf, CL_TRUE, 0, sizeof(int), &changed_int);

                hyst_kernel.setArg(0, *read_buf);
                hyst_kernel.setArg(1, *write_buf);

                queue.enqueueNDRangeKernel(hyst_kernel, cl::NullRange, img_size, cl::NullRange);

                queue.enqueueReadBuffer(changed_buf, CL_TRUE, 0, sizeof(int), &changed_int);
                changed = (changed_int != 0);
                std::swap(read_buf, write_buf);

                if (ENABLE_LOGGING) {
                    std::cout << "Hysteresis iteration " << counter << ", changed: " << changed << std::endl;
                }
                ++counter;
            }

            cl::Kernel suppress_kernel{ program, "canny_suppress_weak" };
            suppress_kernel.setArg(0, *read_buf);
            suppress_kernel.setArg(1, strong);
            suppress_kernel.setArg(2, static_cast<int>(height));
            suppress_kernel.setArg(3, static_cast<int>(width));

            queue.enqueueNDRangeKernel(suppress_kernel, cl::NullRange, img_size, cl::NullRange);

            queue.enqueueReadBuffer(*read_buf, CL_TRUE, 0, img_size * sizeof(float), outputImage.data());
        }

        void CannyEdgeDetectionGPU::processContext(const Context& context) {
            CannyEdgeDetectionComponent::processContext(context);
            img_size = height * width;

            grad_mag_buffer = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            grad_dir_buffer = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            nms_buffer      = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            output_buffer   = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            temp_out_buffer = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };

            queue.enqueueFillBuffer(output_buffer,   0.0f, 0, img_size * sizeof(float));
            queue.enqueueFillBuffer(temp_out_buffer, 0.0f, 0, img_size * sizeof(float));
        }
    } // edge_detection
} // components
