#include "GPUComponent.h"

void GPUComponent::initOpenCL(const std::string& kernel_path) {
    bool ocl_initialized{ oclSetup(clContext, queue, program, kernel_path) };
    if (!ocl_initialized) {
        throw std::runtime_error("Failed to initialize OpenCL context, command queue, or program for kernel: " + kernel_path);
    }
}
