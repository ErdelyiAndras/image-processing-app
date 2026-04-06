#include "SobelEdgeDetectionGPU.h"
#include "SobelEdgeDetectionParameters.h"
#include "Image.h"
#include "kernel_sources.h"

#include <CL/cl.h>
#include <CL/opencl.hpp>

namespace components {
    namespace edge_detection {
        SobelEdgeDetectionGPU::SobelEdgeDetectionGPU()
            : SobelEdgeDetectionGPU(ParamType{}) {}

        SobelEdgeDetectionGPU::SobelEdgeDetectionGPU(const ParamType& params)
            : SobelEdgeDetectionComponent(params) {
            initOpenCL(SOBEL_EDGE_DETECTION_KERNEL_SOURCE);
        }

        SobelEdgeDetectionGPU::SobelEdgeDetectionGPU(float threshold)
            : SobelEdgeDetectionGPU(ParamType{ threshold }) {}

        void SobelEdgeDetectionGPU::applySobelFilter() {
            cl::Buffer img_buffer{ cl_context, CL_MEM_READ_ONLY, img_size * sizeof(float) };
            queue.enqueueWriteBuffer(img_buffer, CL_FALSE, 0, img_size * sizeof(float), inputImage.data());

            cl::Buffer out_buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            queue.enqueueFillBuffer(out_buffer, 0.0f, 0, img_size * sizeof(float));

            cl::Kernel kernel{ program, "sobel_edge_detection" };
            kernel.setArg(0, img_buffer);
            kernel.setArg(1, out_buffer);
            kernel.setArg(2, static_cast<int>(height));
            kernel.setArg(3, static_cast<int>(width));

            queue.enqueueNDRangeKernel(kernel, cl::NullRange, img_size, cl::NullRange);

            cl::Kernel thresh_kernel{ program, "sobel_threshold" };
            thresh_kernel.setArg(0, out_buffer);
            thresh_kernel.setArg(1, parameters.threshold);
            thresh_kernel.setArg(2, static_cast<int>(height));
            thresh_kernel.setArg(3, static_cast<int>(width));

            queue.enqueueNDRangeKernel(thresh_kernel, cl::NullRange, img_size, cl::NullRange);

            queue.enqueueReadBuffer(out_buffer, CL_TRUE, 0, img_size * sizeof(float), outputImage.data());
        }

        void SobelEdgeDetectionGPU::processContext(const Context& context) {
            SobelEdgeDetectionComponent::processContext(context);
            img_size = height * width;
        }
    } // edge_detection
} // components
