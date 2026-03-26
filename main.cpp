#include <CL/opencl.hpp>
#include "oclutils.h"

#include <iostream>
#include <string>
#include <chrono>
#include <unordered_map>

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

#include "Pipeline.h"

#include "CombineShapeMap.h"
#include "CombineEdgeMap.h"

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

        pipeline::Pipeline pipeline;

        NodeId tv_denoising_gpu_node  = pipeline.addNode(std::make_unique<components::denoising::TVDenoisingGPU>());
        NodeId gauss_blur_gpu_node    = pipeline.addNode(std::make_unique<components::denoising::GaussianBlurGPU>());
        NodeId canny_edge_gpu_node    = pipeline.addNode(std::make_unique<components::edge_detection::CannyEdgeDetectionGPU>());
        NodeId sobel_edge_gpu_node    = pipeline.addNode(std::make_unique<components::edge_detection::SobelEdgeDetectionGPU>());
        NodeId hough_line_gpu_node    = pipeline.addNode(std::make_unique<components::shape_detection::HoughLineShapeDetectionGPU>());
        NodeId hough_circle_gpu_node  = pipeline.addNode(std::make_unique<components::shape_detection::HoughCircleShapeDetectionGPU>());
        NodeId combine_shape_map_node = pipeline.addNode(std::make_unique<pipeline::CombineShapeMap>());

        pipeline.connect(tv_denoising_gpu_node, canny_edge_gpu_node);
        pipeline.connect(canny_edge_gpu_node, hough_line_gpu_node);
        pipeline.connect(gauss_blur_gpu_node, sobel_edge_gpu_node);
        pipeline.connect(sobel_edge_gpu_node, hough_circle_gpu_node);
        pipeline.connect(hough_line_gpu_node, combine_shape_map_node);
        pipeline.connect(hough_circle_gpu_node, combine_shape_map_node);

        pipeline.getComponent(tv_denoising_gpu_node).setParameters(
            components::denoising::TVDenoisingParameters{ strength, step_size, tol }
        );
        pipeline.getComponent(gauss_blur_gpu_node).setParameters(
            components::denoising::GaussianBlurParameters{ 5, 1.0f }
        );
        pipeline.getComponent(canny_edge_gpu_node).setParameters(
            components::edge_detection::CannyEdgeDetectionParameters{ 0.2f, 0.4f }
        );
        pipeline.getComponent(sobel_edge_gpu_node).setParameters(
            components::edge_detection::SobelEdgeDetectionParameters{ 0.3f }
        );
        pipeline.getComponent(hough_line_gpu_node).setParameters(
            components::shape_detection::HoughLineShapeDetectionParameters{ 1.0f, pi / 180.0f, 50U, 50U, 100U }
        );
        pipeline.getComponent(hough_circle_gpu_node).setParameters(
            components::shape_detection::HoughCircleShapeDetectionParameters{ 250U, 10U, 100U, 80.0f, 360U }
        );


        NodeId tv_denoising_gpu_node2 = pipeline.addNode(std::make_unique<components::denoising::TVDenoisingGPU>());
        NodeId canny_edge_gpu_node2   = pipeline.addNode(std::make_unique<components::edge_detection::CannyEdgeDetectionGPU>());
        NodeId sobel_edge_gpu_node2   = pipeline.addNode(std::make_unique<components::edge_detection::SobelEdgeDetectionGPU>());
        NodeId hough_line_gpu_node2   = pipeline.addNode(std::make_unique<components::shape_detection::HoughLineShapeDetectionGPU>());
        NodeId hough_circle_gpu_node2 = pipeline.addNode(std::make_unique<components::shape_detection::HoughCircleShapeDetectionGPU>());

        NodeId combine_edge_map_node   = pipeline.addNode(std::make_unique<pipeline::CombineEdgeMap>());
        NodeId combine_shape_map_node2 = pipeline.addNode(std::make_unique<pipeline::CombineShapeMap>());

        pipeline.connect(tv_denoising_gpu_node2, canny_edge_gpu_node2);
        pipeline.connect(tv_denoising_gpu_node2, sobel_edge_gpu_node2);
        pipeline.connect(canny_edge_gpu_node2, combine_edge_map_node);
        pipeline.connect(sobel_edge_gpu_node2, combine_edge_map_node);
        pipeline.connect(combine_edge_map_node, hough_line_gpu_node2);
        pipeline.connect(combine_edge_map_node, hough_circle_gpu_node2);
        pipeline.connect(hough_line_gpu_node2, combine_shape_map_node2);
        pipeline.connect(hough_circle_gpu_node2, combine_shape_map_node2);

        pipeline.getComponent(tv_denoising_gpu_node2).setParameters(
            components::denoising::TVDenoisingParameters{ strength, step_size, tol }
        );
        pipeline.getComponent(canny_edge_gpu_node2).setParameters(
            components::edge_detection::CannyEdgeDetectionParameters{ 0.3f, 0.7f }
        );
        pipeline.getComponent(sobel_edge_gpu_node2).setParameters(
            components::edge_detection::SobelEdgeDetectionParameters{ 0.7f }
        );
        pipeline.getComponent(hough_line_gpu_node2).setParameters(
            components::shape_detection::HoughLineShapeDetectionParameters{ 1.0f, pi / 180.0f, 30U, 10U, 50U }
        );
        pipeline.getComponent(hough_circle_gpu_node2).setParameters(
            components::shape_detection::HoughCircleShapeDetectionParameters{ 250U, 10U, 100U, 80.0f, 360U }
        );


        auto start = std::chrono::high_resolution_clock::now();

        std::unordered_map<NodeId, components::Context> results = pipeline.execute(processing_context);

        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> elapsed = end - start;
        if (ENABLE_LOGGING) {
            std::cout << "Pipeline execution took: " << elapsed.count() << " seconds" << std::endl;
        }

        for (auto& [i, ctx] : results) {
            ctx.save(base, ext);
            ctx.getProcessedImage().save(base + "_processed_" + ctx.getAppliedComponents(), ext);
            ctx.getEdgeMap().save(base + "_edge_" + ctx.getAppliedComponents(), ext);
            ctx.getShapeMap().save(base + "_shape_" + ctx.getAppliedComponents(), ext);
            if (ENABLE_LOGGING) {
                std::cout << "Node " << i << " - " << pipeline.getComponentName(i) << ": output saved to: " << base + "_output_" + ctx.getAppliedComponents() + ext << std::endl;
            }
        }

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
