#ifndef GAUSSIAN_BLUR_PARAMETERS_H
#define GAUSSIAN_BLUR_PARAMETERS_H

#include "Parameters.h"

namespace components {
    namespace denoising {
        struct GaussianBlurParameters : public Parameters {
            int kernel_size;
            float sigma;

            GaussianBlurParameters(int kernel_size, float sigma)
                : kernel_size(kernel_size), sigma(sigma) {}
        };
    } // denoising
} // components

#endif // GAUSSIAN_BLUR_PARAMETERS_H
