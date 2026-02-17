#ifndef DENOISING_CONFIG_H
#define DENOISING_CONFIG_H

namespace components {
    namespace denoising {
        static constexpr const float default_strength  = 0.1f;
        static constexpr const float default_step_size = 1e-2f;
        static constexpr const float default_tolerance = 3.2e-3f;

        static constexpr const float momentum_beta = 0.9f;
        static constexpr const float loss_smoothing_beta = 0.9f;
    } // denoising
} // components

#endif // DENOISING_CONFIG_H
