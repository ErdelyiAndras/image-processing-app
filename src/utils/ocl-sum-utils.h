#ifndef OCL_SUM_UTILS_H
#define OCL_SUM_UTILS_H

#include <CL/opencl.hpp>
#include <string>

template <typename T>
struct SumKernelInfo;

template<>
struct SumKernelInfo<int> {
    static constexpr const char* kernel_name{ "sum_int" };
};

template<>
struct SumKernelInfo<float> {
    static constexpr const char* kernel_name{ "sum_float" };
};

template <typename T>
cl::Kernel init_sum_kernel(cl::Program& program) {
    return cl::Kernel(program, SumKernelInfo<T>::kernel_name);
}

template <typename T>
T sum(cl::Context& context, cl::CommandQueue& queue, cl::Program& program, const T* array, int size) {
    if (size == 0) {
        return static_cast<T>(0);
    }

    cl::Kernel kernel = init_sum_kernel<T>(program);

    int extended_size = 1;
    while (extended_size < size) {
        extended_size *= 2;
    }

    std::vector<T> padded_array(extended_size, static_cast<T>(0));
    std::copy(array, array + size, padded_array.begin());

    cl::Buffer array_buffer(context, CL_MEM_READ_WRITE, extended_size * sizeof(T));
    queue.enqueueWriteBuffer(array_buffer, CL_TRUE, 0, extended_size * sizeof(T), padded_array.data());
    queue.finish();

    kernel.setArg(0, array_buffer);

    for (int offset = extended_size / 2; offset > 0; offset >>= 1) {
        kernel.setArg(1, offset);

        queue.enqueueNDRangeKernel(kernel, cl::NullRange, offset, cl::NullRange);
        queue.finish();
    }

    T result;
    queue.enqueueReadBuffer(array_buffer, CL_TRUE, 0, sizeof(T), &result);

    return result;
}

#endif // OCL_SUM_UTILS_H
