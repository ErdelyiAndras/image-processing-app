#include "TVDenoisingGPU.h"
#include "TVDenoisingParameters.h"
#include "Image.h"
#include "kernel_sources.h"
#include "config.h"
#include "ocl-sum-utils.h"

#include <CL/cl.h>

namespace components {
    namespace denoising {
        TVDenoisingGPU::TVDenoisingGPU()
            : TVDenoisingGPU(ParamType{}) {}

        TVDenoisingGPU::TVDenoisingGPU(const ParamType& params)
            : TVDenoisingComponent(params)
            , inputImageBuffer()
            , outputImageBuffer()
            , tvGradientBuffer()
            , l2GradientBuffer()
            , gradientBuffer()
            , momentumBuffer()
            , tvNormMtxBuffer()
            , dxMtxBuffer()
            , dyMtxBuffer()
            , l2NormMtxBuffer() {
            initOpenCL(TV_DENOISING_KERNEL_SOURCE);
        }

        TVDenoisingGPU::TVDenoisingGPU(float strength, float step_size, float tolerance)
            : TVDenoisingGPU(ParamType{ strength, step_size, tolerance }) {}

        float TVDenoisingGPU::tvNormAndGrad() {
            queue.enqueueFillBuffer(tvNormMtxBuffer, 0.0f, 0, img_size * sizeof(float));
            queue.enqueueFillBuffer(dxMtxBuffer, 0.0f, 0, img_size * sizeof(float));
            queue.enqueueFillBuffer(dyMtxBuffer, 0.0f, 0, img_size * sizeof(float));
            queue.enqueueFillBuffer(tvGradientBuffer, 0.0f, 0, img_size * sizeof(float));

            cl::Kernel kernel{ program, "tv_norm_mtx_and_dx_dy" };
            kernel.setArg(0, outputImageBuffer);
            kernel.setArg(1, tvNormMtxBuffer);
            kernel.setArg(2, dxMtxBuffer);
            kernel.setArg(3, dyMtxBuffer);
            kernel.setArg(4, static_cast<int>(height));
            kernel.setArg(5, static_cast<int>(width));
            kernel.setArg(6, epsilon);

            queue.enqueueNDRangeKernel(kernel, cl::NullRange, img_size, cl::NullRange);

            float tv_norm{ sum<float>(cl_context, queue, utils_program, tvNormMtxBuffer, img_size) };

            std::string kernel_name{ "grad_from_dx_dy" };
            cl::Kernel grad_kernel{ program, kernel_name.c_str() };
            grad_kernel.setArg(0, dxMtxBuffer);
            grad_kernel.setArg(1, dyMtxBuffer);
            grad_kernel.setArg(2, tvGradientBuffer);
            grad_kernel.setArg(3, static_cast<int>(height));
            grad_kernel.setArg(4, static_cast<int>(width));
            queue.enqueueNDRangeKernel(grad_kernel, cl::NullRange, img_size, cl::NullRange);

            return tv_norm;
        }

        float TVDenoisingGPU::l2NormAndGrad() {
            cl::Kernel kernel{ program, "l2_norm_mtx_and_grad" };
            kernel.setArg(0, outputImageBuffer);
            kernel.setArg(1, inputImageBuffer);
            kernel.setArg(2, l2NormMtxBuffer);
            kernel.setArg(3, l2GradientBuffer);
            kernel.setArg(4, static_cast<int>(height));
            kernel.setArg(5, static_cast<int>(width));

            queue.enqueueNDRangeKernel(kernel, cl::NullRange, img_size, cl::NullRange);

            float l2_norm{ sum<float>(cl_context, queue, utils_program, l2NormMtxBuffer, img_size) };

            return 0.5f * l2_norm;
        }

        float TVDenoisingGPU::evalLossAndGrad() {
            const float tv_norm{ tvNormAndGrad() };
            const float l2_norm{ l2NormAndGrad() };

            cl::Kernel kernel{ program, "eval_loss_and_grad" };
            kernel.setArg(0, gradientBuffer);
            kernel.setArg(1, tvGradientBuffer);
            kernel.setArg(2, l2GradientBuffer);
            kernel.setArg(3, parameters.strength);
            queue.enqueueNDRangeKernel(kernel, cl::NullRange, img_size, cl::NullRange);

            return parameters.strength * tv_norm + l2_norm;
        }

        void TVDenoisingGPU::evalMomentumAndUpdateImage(const uint32_t counter) {
            cl::Kernel momentum_kernel{ program, "eval_momentum" };
            momentum_kernel.setArg(0, momentumBuffer);
            momentum_kernel.setArg(1, gradientBuffer);
            momentum_kernel.setArg(2, momentum_beta);
            queue.enqueueNDRangeKernel(momentum_kernel, cl::NullRange, img_size, cl::NullRange);

            cl::Kernel update_kernel{ program, "update_img" };
            update_kernel.setArg(0, outputImageBuffer);
            update_kernel.setArg(1, momentumBuffer);
            update_kernel.setArg(2, step);
            update_kernel.setArg(3, momentum_beta);
            update_kernel.setArg(4, static_cast<float>(counter));
            queue.enqueueNDRangeKernel(update_kernel, cl::NullRange, img_size, cl::NullRange);
        }

        void TVDenoisingGPU::postProcessing() {
            queue.enqueueReadBuffer(outputImageBuffer, CL_TRUE, 0, img_size * sizeof(float), outputImage.data());
        }

        void TVDenoisingGPU::processContext(const Context& context) {
            TVDenoisingComponent::processContext(context);
            img_size = width * height;

            inputImageBuffer  = cl::Buffer{ cl_context, CL_MEM_READ_ONLY,  img_size * sizeof(float) };
            outputImageBuffer = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            tvGradientBuffer  = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            l2GradientBuffer  = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            gradientBuffer    = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            momentumBuffer    = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            tvNormMtxBuffer   = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            dxMtxBuffer       = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            dyMtxBuffer       = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };
            l2NormMtxBuffer   = cl::Buffer{ cl_context, CL_MEM_READ_WRITE, img_size * sizeof(float) };

            queue.enqueueWriteBuffer(inputImageBuffer,  CL_FALSE, 0, img_size * sizeof(float), inputImage.data());
            queue.enqueueWriteBuffer(outputImageBuffer, CL_FALSE, 0, img_size * sizeof(float), outputImage.data());

            queue.enqueueFillBuffer(tvGradientBuffer, 0.0f, 0, img_size * sizeof(float));
            queue.enqueueFillBuffer(l2GradientBuffer, 0.0f, 0, img_size * sizeof(float));
            queue.enqueueFillBuffer(gradientBuffer,   0.0f, 0, img_size * sizeof(float));
            queue.enqueueFillBuffer(momentumBuffer,   0.0f, 0, img_size * sizeof(float));
            queue.enqueueFillBuffer(tvNormMtxBuffer,  0.0f, 0, img_size * sizeof(float));
            queue.enqueueFillBuffer(dxMtxBuffer,      0.0f, 0, img_size * sizeof(float));
            queue.enqueueFillBuffer(dyMtxBuffer,      0.0f, 0, img_size * sizeof(float));
            queue.enqueueFillBuffer(l2NormMtxBuffer,  0.0f, 0, img_size * sizeof(float));
        }
    } // denoising
} // components
