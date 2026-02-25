#include "SobelEdgeDetectionGPU.h"
#include "config.h"

#include <CL/opencl.h>

namespace components {
    namespace edge_detection {
        SobelEdgeDetectionGPU::SobelEdgeDetectionGPU()
            : SobelEdgeDetectionComponent() {
            initOpenCL(SOBEL_EDGE_DETECTION_KERNEL_PATH);
        }

        SobelEdgeDetectionGPU::SobelEdgeDetectionGPU(float threshold)
            : SobelEdgeDetectionComponent(threshold) {
            initOpenCL(SOBEL_EDGE_DETECTION_KERNEL_PATH);
        }

        void SobelEdgeDetectionGPU::applySobelFilter() {
            cl::Kernel kernel{ program, "sobel_edge_detection" };

            cl::Buffer img_buffer{ clContext, CL_MEM_READ_ONLY, img_size * sizeof(float) };
            queue.enqueueWriteBuffer(img_buffer, CL_TRUE, 0, img_size * sizeof(float), inputImage.data());
            queue.finish();

            cl::Buffer out_buffer{ clContext, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            queue.enqueueWriteBuffer(out_buffer, CL_TRUE, 0, img_size * sizeof(float), outputImage.data());
            queue.finish();

            kernel.setArg(0, img_buffer);
            kernel.setArg(1, out_buffer);
            kernel.setArg(2, static_cast<int>(height));
            kernel.setArg(3, static_cast<int>(width));

            queue.enqueueNDRangeKernel(kernel, cl::NullRange, img_size, cl::NullRange);
            queue.finish();

            cl::Kernel thresh_kernel{ program, "sobel_threshold" };
            thresh_kernel.setArg(0, out_buffer);
            thresh_kernel.setArg(1, threshold);
            thresh_kernel.setArg(2, static_cast<int>(height));
            thresh_kernel.setArg(3, static_cast<int>(width));

            queue.enqueueNDRangeKernel(thresh_kernel, cl::NullRange, img_size, cl::NullRange);
            queue.finish();

            queue.enqueueReadBuffer(out_buffer, CL_TRUE, 0, img_size * sizeof(float), outputImage.data());
        }

        void SobelEdgeDetectionGPU::processContext(const Context& context) {
            SobelEdgeDetectionComponent::processContext(context);
            img_size = height * width;
        }
    } // edge_detection
} // components
