#include "TVDenoisingGPU.h"
#include "config.h"
#include "oclutils.h"
#include "ocl-sum-utils.h"

#include <CL/opencl.h>

namespace components {
    namespace denoising {
        TVDenoisingGPU::TVDenoisingGPU()
            : DenoisingComponent()
            , img_size(0U) {
            bool ocl_initialized{ oclSetup(context, queue, program, DENOISING_KERNEL_PATH) };
            if (!ocl_initialized) {
                throw std::runtime_error("Failed to initialize OpenCL context, command queue, or program.");
            }
        }

        TVDenoisingGPU::TVDenoisingGPU(float strength, float step_size, float tolerance)
            : DenoisingComponent(strength, step_size, tolerance)
            , img_size(0U) {
            bool ocl_initialized{ oclSetup(context, queue, program, DENOISING_KERNEL_PATH) };
            if (!ocl_initialized) {
                throw std::runtime_error("Failed to initialize OpenCL context, command queue, or program.");
            }
        }

        float TVDenoisingGPU::tvNormAndGrad() {
            Image tv_norm_mtx(height, width);
            Image dx_mtx(height, width);
            Image dy_mtx(height, width);

            computeTvNormAndDyDyMtxs(tv_norm_mtx, dx_mtx, dy_mtx);

            float tv_norm{ sum(context, queue, program, tv_norm_mtx.data(), img_size) };

            computeGradientFromDxDyMtxs(dx_mtx, dy_mtx);

            return tv_norm;
        }

        float TVDenoisingGPU::l2NormAndGrad() {
            Image l2_norm_mtx(height, width);

            computeL2NormMtxAndGrad(l2_norm_mtx);

            float l2_norm{ sum(context, queue, program, l2_norm_mtx.data(), img_size) };

            return 0.5f * l2_norm;
        }

        float TVDenoisingGPU::evalLossAndGrad() {
            const float tv_norm{ tvNormAndGrad() };

            cl::Kernel kernel(program, "eval_loss_and_grad");

            cl::Buffer grad_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(grad_buffer, CL_TRUE, 0, img_size * sizeof(float), std::vector<float>(img_size, 0.0f).data());
            queue.finish();

            cl::Buffer tv_grad_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(tv_grad_buffer, CL_TRUE, 0, img_size * sizeof(float), tv_gradient.data());
            queue.finish();

            kernel.setArg(0, grad_buffer);
            kernel.setArg(1, tv_grad_buffer);
            kernel.setArg(2, strength);

            queue.enqueueNDRangeKernel(kernel, cl::NullRange, img_size, cl::NullRange);

            const float l2_norm{ l2NormAndGrad() };

            cl::Buffer l2_grad_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(l2_grad_buffer, CL_TRUE, 0, img_size * sizeof(float), l2_gradient.data());
            queue.finish();

            kernel.setArg(1, l2_grad_buffer);
            kernel.setArg(2, 1.0f);

            queue.enqueueNDRangeKernel(kernel, cl::NullRange, img_size, cl::NullRange);

            queue.enqueueReadBuffer(grad_buffer, CL_TRUE, 0, img_size * sizeof(float), gradient.data());

            return strength * tv_norm + l2_norm;
        }

        void TVDenoisingGPU::evalMomentumAndUpdateImage(const uint64_t counter) {
            cl::Kernel momentum_kernel(program, "eval_momentum");

            cl::Buffer momentum_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(momentum_buffer, CL_TRUE, 0, img_size * sizeof(float), momentum.data());
            queue.finish();

            cl::Buffer grad_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(grad_buffer, CL_TRUE, 0, img_size * sizeof(float), gradient.data());
            queue.finish();

            momentum_kernel.setArg(0, momentum_buffer);
            momentum_kernel.setArg(1, grad_buffer);
            momentum_kernel.setArg(2, momentum_beta);

            queue.enqueueNDRangeKernel(momentum_kernel, cl::NullRange, img_size, cl::NullRange);

            queue.enqueueReadBuffer(momentum_buffer, CL_TRUE, 0, img_size * sizeof(float), momentum.data());

            cl::Kernel update_kernel(program, "update_img");

            cl::Buffer img_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(img_buffer, CL_TRUE, 0, img_size * sizeof(float), processedImage.data());
            queue.finish();

            cl::Buffer momentum_buffer2(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(momentum_buffer2, CL_TRUE, 0, img_size * sizeof(float), momentum.data());
            queue.finish();

            update_kernel.setArg(0, img_buffer);
            update_kernel.setArg(1, momentum_buffer2);
            update_kernel.setArg(2, step);
            update_kernel.setArg(3, momentum_beta);
            update_kernel.setArg(4, counter);

            queue.enqueueNDRangeKernel(update_kernel, cl::NullRange, img_size, cl::NullRange);

            queue.enqueueReadBuffer(img_buffer, CL_TRUE, 0, img_size * sizeof(float), processedImage.data());
        }

        void TVDenoisingGPU::computeTvNormAndDyDyMtxs(Image& tv_norm_mtx, Image& dx_mtx, Image& dy_mtx) {
            cl::Kernel kernel(program, "tv_norm_mtx_and_dx_dy");

            cl::Buffer img_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(img_buffer, CL_TRUE, 0, img_size * sizeof(float), processedImage.data());
            queue.finish();

            cl::Buffer tv_norm_mtx_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(tv_norm_mtx_buffer, CL_TRUE, 0, img_size * sizeof(float), std::vector<float>(img_size, 0.0f).data());
            queue.finish();

            cl::Buffer dx_mtx_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(dx_mtx_buffer, CL_TRUE, 0, img_size * sizeof(float), std::vector<float>(img_size, 0.0f).data());
            queue.finish();

            cl::Buffer dy_mtx_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(dy_mtx_buffer, CL_TRUE, 0, img_size * sizeof(float), std::vector<float>(img_size, 0.0f).data());
            queue.finish();

            kernel.setArg(0, img_buffer);
            kernel.setArg(1, tv_norm_mtx_buffer);
            kernel.setArg(2, dx_mtx_buffer);
            kernel.setArg(3, dy_mtx_buffer);
            kernel.setArg(4, height);
            kernel.setArg(5, width);
            kernel.setArg(6, epsilon);

            queue.enqueueNDRangeKernel(kernel, cl::NullRange, img_size, cl::NullRange);

            queue.enqueueReadBuffer(tv_norm_mtx_buffer, CL_TRUE, 0, img_size * sizeof(float), tv_norm_mtx.data());
            queue.enqueueReadBuffer(dx_mtx_buffer, CL_TRUE, 0, img_size * sizeof(float), dx_mtx.data());
            queue.enqueueReadBuffer(dy_mtx_buffer, CL_TRUE, 0, img_size * sizeof(float), dy_mtx.data());
        }

        void TVDenoisingGPU::computeGradientFromDxDyMtxs(Image& dx_mtx, Image& dy_mtx) {
            cl::Buffer dx_mtx_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(dx_mtx_buffer, CL_TRUE, 0, img_size * sizeof(float), dx_mtx.data());
            queue.finish();

            cl::Buffer dy_mtx_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(dy_mtx_buffer, CL_TRUE, 0, img_size * sizeof(float), dy_mtx.data());
            queue.finish();

            cl::Buffer grad_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(grad_buffer, CL_TRUE, 0, img_size * sizeof(float), std::vector<float>(img_size, 0.0f).data());
            queue.finish();

            for (int i = 1; i <= 3; ++i) {
                std::string kernel_name = "grad_from_dx_dy_step" + std::to_string(i);
                cl::Kernel kernel(program, kernel_name.c_str());

                kernel.setArg(0, dx_mtx_buffer);
                kernel.setArg(1, dy_mtx_buffer);
                kernel.setArg(2, grad_buffer);
                kernel.setArg(3, height);
                kernel.setArg(4, width);

                queue.enqueueNDRangeKernel(kernel, cl::NullRange, img_size, cl::NullRange);
                queue.finish();
            }
            queue.enqueueReadBuffer(grad_buffer, CL_TRUE, 0, img_size * sizeof(float), tv_gradient.data());
        }

        void TVDenoisingGPU::computeL2NormMtxAndGrad(Image& l2_norm_mtx) {
            cl::Kernel kernel(program, "l2_norm_mtx_and_grad");

            cl::Buffer img_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(img_buffer, CL_TRUE, 0, img_size * sizeof(float), processedImage.data());
            queue.finish();

            cl::Buffer orig_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(orig_buffer, CL_TRUE, 0, img_size * sizeof(float), startImage.data());
            queue.finish();

            cl::Buffer l2_norm_mtx_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(l2_norm_mtx_buffer, CL_TRUE, 0, img_size * sizeof(float), l2_norm_mtx.data());
            queue.finish();

            cl::Buffer grad_buffer(context, CL_MEM_READ_WRITE, img_size * sizeof(float));
            queue.enqueueWriteBuffer(grad_buffer, CL_TRUE, 0, img_size * sizeof(float), l2_gradient.data());
            queue.finish();

            kernel.setArg(0, img_buffer);
            kernel.setArg(1, orig_buffer);
            kernel.setArg(2, l2_norm_mtx_buffer);
            kernel.setArg(3, grad_buffer);
            kernel.setArg(4, height);
            kernel.setArg(5, width);

            queue.enqueueNDRangeKernel(kernel, cl::NullRange, img_size, cl::NullRange);

            queue.enqueueReadBuffer(l2_norm_mtx_buffer, CL_TRUE, 0, img_size * sizeof(float), l2_norm_mtx.data());
            queue.enqueueReadBuffer(grad_buffer, CL_TRUE, 0, img_size * sizeof(float), l2_gradient.data());
        }

        void TVDenoisingGPU::processContext(const Context& context) {
            DenoisingComponent::processContext(context);
            img_size = width * height;
        }
    } // denoising
} // components
