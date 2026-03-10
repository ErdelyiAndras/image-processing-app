#include "GaussianBlurGPU.h"
#include "config.h"

#include <CL/opencl.h>

#include <cmath>
#include <vector>

namespace components {
    namespace denoising {
        GaussianBlurGPU::GaussianBlurGPU()
            : GaussianBlurComponent() {
            initOpenCL(GAUSSIAN_BLUR_KERNEL_PATH);
        }

        GaussianBlurGPU::GaussianBlurGPU(int kernel_size, float sigma)
            : GaussianBlurComponent(kernel_size, sigma) {
            initOpenCL(GAUSSIAN_BLUR_KERNEL_PATH);
        }

        void GaussianBlurGPU::computeConvolution(const std::vector<float>& kernel) {
            cl::Buffer img_buf{ cl_context, CL_MEM_READ_ONLY, img_size * sizeof(float) };
            queue.enqueueWriteBuffer(img_buf, CL_FALSE, 0, img_size * sizeof(float), inputImage.data());

            cl::Buffer out_buf{ cl_context, CL_MEM_WRITE_ONLY, img_size * sizeof(float) };

            cl::Buffer kernel_buf{ cl_context, CL_MEM_READ_ONLY, kernel.size() * sizeof(float) };
            queue.enqueueWriteBuffer(kernel_buf, CL_FALSE, 0, kernel.size() * sizeof(float), kernel.data());

            cl::Kernel gpu_kernel{ program, "gaussian_blur" };
            gpu_kernel.setArg(0, img_buf);
            gpu_kernel.setArg(1, out_buf);
            gpu_kernel.setArg(2, kernel_buf);
            gpu_kernel.setArg(3, height);
            gpu_kernel.setArg(4, width);
            gpu_kernel.setArg(5, kernel_size);

            queue.enqueueNDRangeKernel(gpu_kernel, cl::NullRange, img_size, cl::NullRange);

            queue.enqueueReadBuffer(out_buf, CL_TRUE, 0, img_size * sizeof(float), outputImage.data());
        }

        void GaussianBlurGPU::processContext(const Context& context) {
            GaussianBlurComponent::processContext(context);
            img_size = height * width;
        }
    } // denoising
} // components
