#ifndef NODE_TYPES_H
#define NODE_TYPES_H

#include "TVDenoisingParameters.h"
#include "GaussianBlurParameters.h"
#include "SobelEdgeDetectionParameters.h"
#include "CannyEdgeDetectionParameters.h"
#include "HoughLineShapeDetectionParameters.h"
#include "HoughCircleShapeDetectionParameters.h"

#include <string>
#include <variant>

using TVParams     = components::denoising::TVDenoisingParameters;
using GaussParams  = components::denoising::GaussianBlurParameters;
using SobelParams  = components::edge_detection::SobelEdgeDetectionParameters;
using CannyParams  = components::edge_detection::CannyEdgeDetectionParameters;
using HoughLParams = components::shape_detection::HoughLineShapeDetectionParameters;
using HoughCParams = components::shape_detection::HoughCircleShapeDetectionParameters;

using NodeParams = std::variant<
    std::monostate,
    TVParams,
    GaussParams,
    SobelParams,
    CannyParams,
    HoughLParams,
    HoughCParams
>;

enum class ComponentType {
    TVDenoisingCPU,
    TVDenoisingGPU,
    GaussianBlurCPU,
    GaussianBlurGPU,
    SobelCPU,
    SobelGPU,
    CannyCPU,
    CannyGPU,
    HoughLineCPU,
    HoughLineGPU,
    HoughCircleCPU,
    HoughCircleGPU,
    CombineEdgeMap,
    CombineShapeMap
};

struct NodeInfo {
    ComponentType type;
    std::string   displayName;
    NodeParams    params;
};

#endif // NODE_TYPES_H
