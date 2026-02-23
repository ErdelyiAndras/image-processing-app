#ifndef TV_DENOISING_GPU_H
#define TV_DENOISING_GPU_H

#include "TVDenoisingComponent.h"
#include "GPUComponent.h"

namespace components {
    namespace denoising {
        class TVDenoisingGPU : public TVDenoisingComponent, protected GPUComponent {
        public:
            TVDenoisingGPU();
            TVDenoisingGPU(float strength, float step_size, float tolerance);

        private:
            float tvNormAndGrad() override;
            float l2NormAndGrad() override;
            float evalLossAndGrad() override;
            void  evalMomentumAndUpdateImage(const uint64_t counter) override;

            void computeTvNormAndDyDyMtxs(Image& tv_norm_mtx, Image& dx_mtx, Image& dy_mtx);
            void computeGradientFromDxDyMtxs(Image& dx_mtx, Image& dy_mtx);
            void computeL2NormMtxAndGrad(Image& l2_norm_mtx);

            void processContext(const Context& context) override;
        };
    } // denoising
} // components

#endif // TV_DENOISING_GPU_H
