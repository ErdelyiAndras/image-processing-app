#include "GPUComponent.h"

#include <iostream>

cl::Context GPUComponent::s_context;
cl::CommandQueue GPUComponent::s_queue;
std::vector<cl::Device> GPUComponent::s_devices;
bool GPUComponent::s_is_initialized = false;

bool GPUComponent::ensureInitialized() {
    if (s_is_initialized) {
        return true;
    }

    if (!oclCreateContextBy(s_context)) {
        if (ENABLE_LOGGING) {
            std::cerr << "Failed to create shared OpenCL context" << std::endl;
        }
        return false;
    }

    s_devices = s_context.getInfo<CL_CONTEXT_DEVICES>();
    s_queue = cl::CommandQueue(s_context, s_devices[0], CL_QUEUE_PROFILING_ENABLE);
    s_is_initialized = true;
    return true;
}

void GPUComponent::initOpenCL(const std::string& kernel_path) {
    if (!ensureInitialized()) {
        throw std::runtime_error("Failed to initialize shared OpenCL context for kernel: " + kernel_path);
    }

    cl_context = s_context;
    queue = s_queue;

    std::string source_code = oclReadSourcesFromFile(kernel_path);
    program = cl::Program(s_context, source_code);

    try {
        program.build(s_devices);
    }
    catch (const cl::Error& error) {
        oclPrintError(error);
        if (ENABLE_LOGGING) {
            std::cerr << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(s_devices[0]) << std::endl;
            std::cerr << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(s_devices[0]) << std::endl;
            std::cerr << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(s_devices[0]) << std::endl;
        }
        throw std::runtime_error("Failed to build OpenCL program for kernel: " + kernel_path);
    }
}
