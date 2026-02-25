#ifndef GPU_COMPONENT_H
#define GPU_COMPONENT_H

#include <CL/opencl.hpp>
#include <stdexcept>
#include <string>
#include <vector>

#include "types.h"
#include "oclutils.h"

class GPUComponent {
public:
    virtual ~GPUComponent() = default;

protected:
    PixelIdx img_size{ 0U };
    cl::Context cl_context;
    cl::CommandQueue queue;
    cl::Program program;

    void initOpenCL(const std::string& kernel_path);

private:
    static cl::Context s_context;
    static cl::CommandQueue s_queue;
    static std::vector<cl::Device> s_devices;
    static bool s_is_initialized;

    static bool ensureInitialized();
};

#endif // GPU_COMPONENT_H
