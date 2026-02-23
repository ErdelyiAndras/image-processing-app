#ifndef GPU_COMPONENT_H
#define GPU_COMPONENT_H

#include <CL/opencl.hpp>
#include <stdexcept>
#include <string>

#include "types.h"
#include "oclutils.h"

class GPUComponent {
public:
    virtual ~GPUComponent() = default;

protected:
    PixelIdx img_size{ 0U };
    cl::Context clContext;
    cl::CommandQueue queue;
    cl::Program program;

    void initOpenCL(const std::string& kernel_path);
};

#endif // GPU_COMPONENT_H
