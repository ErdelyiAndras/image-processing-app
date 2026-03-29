#ifndef NODE_FACTORY_H
#define NODE_FACTORY_H

#include "NodeTypes.h"
#include "MergeStrategy.h"
#include "Component.h"

#include "TVDenoisingCPU.h"
#include "TVDenoisingGPU.h"
#include "GaussianBlurCPU.h"
#include "GaussianBlurGPU.h"
#include "SobelEdgeDetectionCPU.h"
#include "SobelEdgeDetectionGPU.h"
#include "CannyEdgeDetectionCPU.h"
#include "CannyEdgeDetectionGPU.h"
#include "HoughLineShapeDetectionCPU.h"
#include "HoughLineShapeDetectionGPU.h"
#include "HoughCircleShapeDetectionCPU.h"
#include "HoughCircleShapeDetectionGPU.h"
#include "CombineEdgeMap.h"
#include "CombineShapeMap.h"

#include <memory>
#include <stdexcept>

class NodeFactory {
public:
    NodeFactory() = delete;

    static const char* typeName(ComponentType t) {
        switch (t) {
            case ComponentType::TVDenoisingCPU:  return "TV Denoising (CPU)";
            case ComponentType::TVDenoisingGPU:  return "TV Denoising (GPU)";
            case ComponentType::GaussianBlurCPU: return "Gaussian Blur (CPU)";
            case ComponentType::GaussianBlurGPU: return "Gaussian Blur (GPU)";
            case ComponentType::SobelCPU:        return "Sobel Edge Detection (CPU)";
            case ComponentType::SobelGPU:        return "Sobel Edge Detection (GPU)";
            case ComponentType::CannyCPU:        return "Canny Edge Detection (CPU)";
            case ComponentType::CannyGPU:        return "Canny Edge Detection (GPU)";
            case ComponentType::HoughLineCPU:    return "Hough Line Detection (CPU)";
            case ComponentType::HoughLineGPU:    return "Hough Line Detection (GPU)";
            case ComponentType::HoughCircleCPU:  return "Hough Circle Detection (CPU)";
            case ComponentType::HoughCircleGPU:  return "Hough Circle Detection (GPU)";
            case ComponentType::CombineEdgeMap:  return "Combine Edge Map";
            case ComponentType::CombineShapeMap: return "Combine Shape Map";
            default:                             return "Unknown";
        }
    }

    static bool isMerge(ComponentType t) {
        return t == ComponentType::CombineEdgeMap ||
               t == ComponentType::CombineShapeMap;
    }

    static std::unique_ptr<components::Component> makeComponent(
        ComponentType type, const NodeParams& params
    ) {
        switch (type) {
            case ComponentType::TVDenoisingCPU: {
                const TVParams& p{ std::get<TVParams>(params) };
                return std::make_unique<components::denoising::TVDenoisingCPU>(
                    p.strength, p.step_size, p.tolerance
                );
            }
            case ComponentType::TVDenoisingGPU: {
                const TVParams& p{ std::get<TVParams>(params) };
                return std::make_unique<components::denoising::TVDenoisingGPU>(
                    p.strength, p.step_size, p.tolerance
                );
            }
            case ComponentType::GaussianBlurCPU: {
                const GaussParams& p{ std::get<GaussParams>(params) };
                return std::make_unique<components::denoising::GaussianBlurCPU>(
                    p.kernel_size, p.sigma
                );
            }
            case ComponentType::GaussianBlurGPU: {
                const GaussParams& p{ std::get<GaussParams>(params) };
                return std::make_unique<components::denoising::GaussianBlurGPU>(
                    p.kernel_size, p.sigma
                );
            }
            case ComponentType::SobelCPU: {
                const SobelParams& p{ std::get<SobelParams>(params) };
                return std::make_unique<components::edge_detection::SobelEdgeDetectionCPU>(
                    p.threshold
                );
            }
            case ComponentType::SobelGPU: {
                const SobelParams& p{ std::get<SobelParams>(params) };
                return std::make_unique<components::edge_detection::SobelEdgeDetectionGPU>(
                    p.threshold
                );
            }
            case ComponentType::CannyCPU: {
                const CannyParams& p{ std::get<CannyParams>(params) };
                return std::make_unique<components::edge_detection::CannyEdgeDetectionCPU>(
                    p.low_threshold, p.high_threshold
                );
            }
            case ComponentType::CannyGPU: {
                const CannyParams& p{ std::get<CannyParams>(params) };
                return std::make_unique<components::edge_detection::CannyEdgeDetectionGPU>(
                    p.low_threshold, p.high_threshold
                );
            }
            case ComponentType::HoughLineCPU: {
                const HoughLParams& p{ std::get<HoughLParams>(params) };
                return std::make_unique<components::shape_detection::HoughLineShapeDetectionCPU>(
                    p.rho_resolution, p.theta_resolution,
                    p.vote_min_threshold, p.min_line_length, p.max_line_gap
                );
            }
            case ComponentType::HoughLineGPU: {
                const HoughLParams& p{ std::get<HoughLParams>(params) };
                return std::make_unique<components::shape_detection::HoughLineShapeDetectionGPU>(
                    p.rho_resolution, p.theta_resolution,
                    p.vote_min_threshold, p.min_line_length, p.max_line_gap
                );
            }
            case ComponentType::HoughCircleCPU: {
                const HoughCParams& p{ std::get<HoughCParams>(params) };
                return std::make_unique<components::shape_detection::HoughCircleShapeDetectionCPU>(
                    p.vote_min_threshold, p.min_radius,
                    p.max_radius, p.min_dist, p.num_angle_steps
                );
            }
            case ComponentType::HoughCircleGPU: {
                const HoughCParams& p{ std::get<HoughCParams>(params) };
                return std::make_unique<components::shape_detection::HoughCircleShapeDetectionGPU>(
                    p.vote_min_threshold, p.min_radius,
                    p.max_radius, p.min_dist, p.num_angle_steps
                );
            }
            default:
                throw std::invalid_argument("NodeFactory::makeComponent: unhandled ComponentType");
        }
    }

    static std::unique_ptr<pipeline::MergeStrategy> makeMergeStrategy(ComponentType type) {
        switch (type) {
            case ComponentType::CombineEdgeMap:
                return std::make_unique<pipeline::CombineEdgeMap>();
            case ComponentType::CombineShapeMap:
                return std::make_unique<pipeline::CombineShapeMap>();
            default:
                throw std::invalid_argument{ "NodeFactory::makeMergeStrategy: not a merge type" };
        }
    }
};

#endif // NODE_FACTORY_H
