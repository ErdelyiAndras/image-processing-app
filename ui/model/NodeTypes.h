#ifndef NODE_TYPES_H
#define NODE_TYPES_H

#include "types.h"

#include "GaussianBlurParameters.h"
#include "TVDenoisingParameters.h"
#include "CannyEdgeDetectionParameters.h"
#include "SobelEdgeDetectionParameters.h"
#include "HoughCircleShapeDetectionParameters.h"
#include "HoughLineShapeDetectionParameters.h"

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

enum class ComponentType : uint8_t {
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
    CombineShapeMap,
    ComponentTypeCount
};

enum class Category : uint8_t {
    Denoising,
    EdgeDetection,
    ShapeDetection,
    Merge,
    CategoryCount
};

struct NodeInfo {
    ComponentType type;
    std::string   displayName;
    NodeParams    params;
};

#endif // NODE_TYPES_H
