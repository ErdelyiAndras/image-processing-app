#include <CL/opencl.hpp>
#include "oclutils.h"

#include <iostream>
#include <string>
#include <chrono>

#include "Image.h"
#include "TVDenoisingGPU.h"
#include "TVDenoisingCPU.h"

int main(int argc, char** argv) {
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] 
                  << " <input_image_path> <output_image_path> <strength> <step_size> <tol> <suppress_log>" 
                  << std::endl;
        return -1;
    }
    std::string suppress_log_str = argv[6];
    bool suppress_log = true;
    if (suppress_log_str == "false" || suppress_log_str == "0") {
        suppress_log = false;
    }

    try {
        Image image(argv[1]);
        int img_size = image.getRows() * image.getCols();

        cl::Context context;
        if (!oclCreateContextBy(context, "intel")) {
            throw cl::Error(CL_INVALID_CONTEXT, "Failed to create a valid context!");
        }

        cl::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
        cl::CommandQueue queue(context, devices[0], CL_QUEUE_PROFILING_ENABLE);


        std::string denoising_kernel_path = DENOISING_KERNEL_PATH;
        std::string source_code = oclReadSourcesFromFile(denoising_kernel_path);
        cl::Program program(context, source_code);

        try {
            program.build(devices);
        }
        catch (cl::Error error) {
            oclPrintError(error);
            std::cerr << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) << std::endl;
            std::cerr << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]) << std::endl;
            std::cerr << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
            return -1;
        }

        float strength = std::stof(argv[3]);
        float step_size = std::stof(argv[4]);
        float tol = std::stof(argv[5]);

        auto start = std::chrono::high_resolution_clock::now();

        Image denoisedImage_gpu = tv_denoise_gradient_descent(context, queue, program, image, strength, step_size, tol, suppress_log);
        
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<float> elapsed = end - start;
        std::cout << "GPU_Denoising took: " << elapsed.count() << " seconds" << std::endl;
        
        start = std::chrono::high_resolution_clock::now();

        Image denoisedImage_cpu = tv_denoise_gradient_descent(image, strength, step_size, tol, suppress_log);
        
        end = std::chrono::high_resolution_clock::now();

        elapsed = end - start;
        std::cout << "CPU_Denoising took: " << elapsed.count() << " seconds" << std::endl;
        
        std::string output_path = argv[2];
        size_t dot_pos = output_path.find_last_of('.');
        std::string base = (dot_pos == std::string::npos) ? output_path : output_path.substr(0, dot_pos);
        std::string ext = (dot_pos == std::string::npos) ? "" : output_path.substr(dot_pos);

        denoisedImage_gpu.save(base + "_gpu" + ext);
        denoisedImage_cpu.save(base + "_cpu" + ext);
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cout << "Other exception" << std::endl;
        return -1;
    }
    
    return 0;
}
