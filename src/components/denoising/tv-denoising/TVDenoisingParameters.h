#ifndef TV_DENOISING_PARAMETERS_H
#define TV_DENOISING_PARAMETERS_H

#include "Parameters.h"

namespace components {
    namespace denoising {
        struct TVDenoisingParameters : public Parameters {
            float strength;
            float step_size;
            float tolerance;

            TVDenoisingParameters(float strength, float step_size, float tolerance)
                : strength(strength), step_size(step_size), tolerance(tolerance) {}
        };
    } // denoising
} // components

#endif // TV_DENOISING_PARAMETERS_H
