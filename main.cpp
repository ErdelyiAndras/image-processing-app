#include <CL/opencl.hpp>
#include "oclutils.h"

#include <iostream>
#include <string>
#include <chrono>

#include "Image.h"

#include "TVDenoisingGPU.h"
#include "TVDenoisingCPU.h"

#include "TVDenoisingParameters.h"

#include "GaussianBlurCPU.h"
#include "GaussianBlurGPU.h"

#include "GaussianBlurParameters.h"

#include "SobelEdgeDetectionCPU.h"
#include "SobelEdgeDetectionGPU.h"

#include "SobelEdgeDetectionParameters.h"

#include "CannyEdgeDetectionCPU.h"
#include "CannyEdgeDetectionGPU.h"

#include "CannyEdgeDetectionParameters.h"

#include "HoughLineShapeDetectionCPU.h"
#include "HoughLineShapeDetectionGPU.h"

#include "HoughLineShapeDetectionParameters.h"

#include "HoughCircleShapeDetectionCPU.h"
#include "HoughCircleShapeDetectionGPU.h"

#include "HoughCircleShapeDetectionParameters.h"

#include "Component.h"
#include "Context.h"

template<typename ComponentType, typename ParameterType>
void useComponent(const ParameterType& params, components::Context& context, const std::string& component_name, const std::string& base, const std::string& ext) {
    std::unique_ptr<components::Component> denoiser = std::make_unique<ComponentType>();
    denoiser->setParameters(params);

    if (ENABLE_LOGGING) {
        std::cout << "Starting component: " << component_name << std::endl;
    }

    auto start = std::chrono::high_resolution_clock::now();

    denoiser->process(context);

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> elapsed = end - start;
    if (ENABLE_LOGGING) {
        std::cout << component_name << " took: " << elapsed.count() << " seconds" << std::endl;
    }

    context.getProcessedImage().save(base + "_" + component_name + ext);
}

int main(int argc, char** argv) {
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0]
                  << " <input_image_path> <output_image_path> <strength> <step_size> <tol>"
                  << std::endl;
        return -1;
    }

    try {
        Image image(argv[1]);
        float strength = std::stof(argv[3]);
        float step_size = std::stof(argv[4]);
        float tol = std::stof(argv[5]);

        std::string output_path = argv[2];
        size_t dot_pos = output_path.find_last_of('.');
        std::string base = (dot_pos == std::string::npos) ? output_path : output_path.substr(0, dot_pos);
        std::string ext = (dot_pos == std::string::npos) ? "" : output_path.substr(dot_pos);

        components::Context processing_context{ image };


        #pragma region TV Denoising

        components::denoising::TVDenoisingParameters tv_params{ strength, step_size, tol };

        // useComponent<
        //     components::denoising::TVDenoisingCPU,
        //     components::denoising::TVDenoisingParameters
        // >(tv_params, processing_context, "tv_denoise_cpu", base, ext);

        // useComponent<
        //     components::denoising::TVDenoisingGPU,
        //     components::denoising::TVDenoisingParameters
        // >(tv_params, processing_context, "tv_denoise_gpu", base, ext);

        #pragma endregion


        // #pragma region Gaussian Blur

        components::denoising::GaussianBlurParameters gauss_params{ 5, 3.0f };

        // useComponent<
        //     components::denoising::GaussianBlurCPU,
        //     components::denoising::GaussianBlurParameters
        // >(gauss_params, processing_context, "gaussian_blur_cpu", base, ext);

        // useComponent<
        //     components::denoising::GaussianBlurGPU,
        //     components::denoising::GaussianBlurParameters
        // >(gauss_params, processing_context, "gaussian_blur_gpu", base, ext);

        // #pragma endregion


        #pragma region Sobel Edge Detection

        components::edge_detection::SobelEdgeDetectionParameters sobel_params{ 0.2f };

        // useComponent<
        //     components::edge_detection::SobelEdgeDetectionCPU,
        //     components::edge_detection::SobelEdgeDetectionParameters
        // >(sobel_params, processing_context, "sobel_edge_cpu", base, ext);

        // useComponent<
        //     components::edge_detection::SobelEdgeDetectionGPU,
        //     components::edge_detection::SobelEdgeDetectionParameters
        // >(sobel_params, processing_context, "sobel_edge_gpu", base, ext);

        #pragma endregion

        #pragma region Canny Edge Detection

        components::edge_detection::CannyEdgeDetectionParameters canny_params{ 0.2f, 0.5f };

        // useComponent<
        //     components::edge_detection::CannyEdgeDetectionCPU,
        //     components::edge_detection::CannyEdgeDetectionParameters
        // >(canny_params, processing_context, "canny_edge_cpu", base, ext);

        // useComponent<
        //     components::edge_detection::CannyEdgeDetectionGPU,
        //     components::edge_detection::CannyEdgeDetectionParameters
        // >(canny_params, processing_context, "canny_edge_gpu", base, ext);

        #pragma endregion

        #pragma region Hough Line Shape Detection

        components::shape_detection::HoughLineShapeDetectionParameters hough_line_params{ 10.0f, static_cast<float>(pi) / 360.0f, 100U, 300U, 50U };

        // useComponent<
        //     components::shape_detection::HoughLineShapeDetectionCPU,
        //     components::shape_detection::HoughLineShapeDetectionParameters
        // >(hough_line_params, processing_context, "hough_line_cpu", base, ext);

        // useComponent<
        //     components::shape_detection::HoughLineShapeDetectionGPU,
        //     components::shape_detection::HoughLineShapeDetectionParameters
        // >(hough_line_params, processing_context, "hough_line_gpu", base, ext);

        #pragma endregion

        #pragma region Hough Circle Shape Detection

        components::shape_detection::HoughCircleShapeDetectionParameters hough_circle_params{ 30U, 100U, 200U, 20.0f, 360U };

        // useComponent<
        //     components::shape_detection::HoughCircleShapeDetectionCPU,
        //     components::shape_detection::HoughCircleShapeDetectionParameters
        // >(hough_circle_params, processing_context, "hough_circle_gpu", base, ext);

        // useComponent<
        //     components::shape_detection::HoughCircleShapeDetectionCPU,
        //     components::shape_detection::HoughCircleShapeDetectionParameters
        // >(hough_circle_params, processing_context, "hough_circle_cpu", base, ext);

        #pragma endregion
    }
    catch (const std::exception& e) {
        if (ENABLE_LOGGING) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
        return -1;
    }
    catch (...) {
        if (ENABLE_LOGGING) {
            std::cerr << "Other exception" << std::endl;
        }
        return -1;
    }

    return 0;
}
