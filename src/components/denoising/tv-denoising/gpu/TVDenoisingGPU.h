#ifndef TV_DENOISING_GPU_H
#define TV_DENOISING_GPU_H

#include "TVDenoisingComponent.h"
#include "TVDenoisingParameters.h"
#include "GPUComponent.h"

#include <string>

namespace components {
    namespace denoising {
        class TVDenoisingGPU final : public TVDenoisingComponent, protected GPUComponent {
        public:
            TVDenoisingGPU();
            explicit TVDenoisingGPU(const TVDenoisingParameters& params);
            explicit TVDenoisingGPU(float strength, float step_size, float tolerance);

            std::string getName() const override { return "tv-gpu"; }

        private:
            cl::Buffer inputImageBuffer;
            cl::Buffer outputImageBuffer;
            cl::Buffer tvGradientBuffer;
            cl::Buffer l2GradientBuffer;
            cl::Buffer gradientBuffer;
            cl::Buffer momentumBuffer;
            cl::Buffer tvNormMtxBuffer;
            cl::Buffer dxMtxBuffer;
            cl::Buffer dyMtxBuffer;
            cl::Buffer l2NormMtxBuffer;

            float tvNormAndGrad() override;
            float l2NormAndGrad() override;
            float evalLossAndGrad() override;
            void  evalMomentumAndUpdateImage(const uint32_t counter) override;
            void  postProcessing() override;

            void processContext(const Context& context) override;
        };
    } // denoising
} // components

#endif // TV_DENOISING_GPU_H
