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

        void GaussianBlurGPU::applyGaussianBlur() {
            int half{ kernel_size / 2 };

            // Generate 2D Gaussian kernel on host
            int kernel_total{ kernel_size * kernel_size };
            std::vector<float> kernel_data(kernel_total);
            float sum{ 0.0f };
            for (int i{ -half }; i <= half; ++i) {
                for (int j{ -half }; j <= half; ++j) {
                    float val{
                        std::exp(-(static_cast<float>(i * i + j * j)) /
                        (2.0f * sigma * sigma))
                    };
                    kernel_data[(i + half) * kernel_size + (j + half)] = val;
                    sum += val;
                }
            }
            for (float& v : kernel_data) {
                v /= sum;
            }

            cl::Buffer img_buf(clContext, CL_MEM_READ_ONLY, img_size * sizeof(float));
            queue.enqueueWriteBuffer(img_buf, CL_TRUE, 0, img_size * sizeof(float), inputImage.data());
            queue.finish();

            cl::Buffer out_buf(clContext, CL_MEM_WRITE_ONLY, img_size * sizeof(float));
            queue.enqueueWriteBuffer(out_buf, CL_TRUE, 0, img_size * sizeof(float), std::vector<float>(img_size, 0.0f).data());
            queue.finish();

            cl::Buffer kernel_buf(clContext, CL_MEM_READ_ONLY, kernel_total * sizeof(float));
            queue.enqueueWriteBuffer(kernel_buf, CL_TRUE, 0, kernel_total * sizeof(float), kernel_data.data());
            queue.finish();

            cl::Kernel gpu_kernel(program, "gaussian_blur");
            gpu_kernel.setArg(0, img_buf);
            gpu_kernel.setArg(1, out_buf);
            gpu_kernel.setArg(2, kernel_buf);
            gpu_kernel.setArg(3, height);
            gpu_kernel.setArg(4, width);
            gpu_kernel.setArg(5, kernel_size);

            queue.enqueueNDRangeKernel(gpu_kernel, cl::NullRange, img_size, cl::NullRange);
            queue.finish();

            queue.enqueueReadBuffer(out_buf, CL_TRUE, 0, img_size * sizeof(float), outputImage.data());
        }

        void GaussianBlurGPU::processContext(const Context& context) {
            DenoisingComponent::processContext(context);
            img_size = height * width;
        }
    } // denoising
} // components
