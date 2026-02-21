#ifndef DENOISING_PARAMETERS_H
#define DENOISING_PARAMETERS_H

#include "Parameters.h"

namespace components {
    namespace denoising {
        struct DenoisingParameters : public Parameters {
            float strength;
            float step_size;
            float tolerance;

            DenoisingParameters(float strength, float step_size, float tolerance)
                : strength(strength), step_size(step_size), tolerance(tolerance) {}
        };
    }
}

#endif // DENOISING_PARAMETERS_H
