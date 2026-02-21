#include <CL/opencl.hpp>
#include "oclutils.h"

#include <iostream>
#include <string>
#include <chrono>

#include "Image.h"
#include "TVDenoisingGPU.h"
#include "TVDenoisingCPU.h"

#include "Component.h"
#include "Context.h"

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
        float strength = std::stof(argv[3]);
        float step_size = std::stof(argv[4]);
        float tol = std::stof(argv[5]);

        std::unique_ptr<components::Component> denoiser = std::make_unique<components::denoising::TVDenoisingCPU>(strength, step_size, tol);
        components::Context cpu_processing_context{ image };

        auto start = std::chrono::high_resolution_clock::now();

        denoiser->process(cpu_processing_context);

        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> elapsed = end - start;
        std::cout << "CPU_Denoising took: " << elapsed.count() << " seconds" << std::endl;


        denoiser = std::make_unique<components::denoising::TVDenoisingGPU>(strength, step_size, tol);
        components::Context gpu_processing_context{ image };

        start = std::chrono::high_resolution_clock::now();

        denoiser->process(gpu_processing_context);
        
        end = std::chrono::high_resolution_clock::now();

        elapsed = end - start;
        std::cout << "GPU_Denoising took: " << elapsed.count() << " seconds" << std::endl;

        std::string output_path = argv[2];
        size_t dot_pos = output_path.find_last_of('.');
        std::string base = (dot_pos == std::string::npos) ? output_path : output_path.substr(0, dot_pos);
        std::string ext = (dot_pos == std::string::npos) ? "" : output_path.substr(dot_pos);

        cpu_processing_context.getProcessedImage().save(base + "_cpu" + ext);
        gpu_processing_context.getProcessedImage().save(base + "_gpu" + ext);
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
