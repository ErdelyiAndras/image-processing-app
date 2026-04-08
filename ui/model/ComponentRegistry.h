#ifndef COMPONENT_REGISTRY_H
#define COMPONENT_REGISTRY_H

#include "Component.h"
#include "MergeStrategy.h"
#include "NodeTypes.h"

#include "GaussianBlurCPU.h"
#include "GaussianBlurGPU.h"
#include "TVDenoisingCPU.h"
#include "TVDenoisingGPU.h"
#include "CannyEdgeDetectionCPU.h"
#include "CannyEdgeDetectionGPU.h"
#include "SobelEdgeDetectionCPU.h"
#include "SobelEdgeDetectionGPU.h"
#include "HoughCircleShapeDetectionCPU.h"
#include "HoughCircleShapeDetectionGPU.h"
#include "HoughLineShapeDetectionCPU.h"
#include "HoughLineShapeDetectionGPU.h"
#include "CombineEdgeMap.h"
#include "CombineShapeMap.h"

#include <array>
#include <memory>
#include <stdexcept>
#include <vector>

class ComponentDescriptor {
public:
    virtual ~ComponentDescriptor() = default;

    virtual ComponentType type()        const = 0;
    virtual const char*   displayName() const = 0;
    virtual Category      category()    const = 0;

    virtual bool isMerge()   const = 0;
    virtual bool hasParams() const = 0;

    virtual NodeParams defaultParams() const = 0;

    virtual std::unique_ptr<components::Component>   makeComponent(const NodeParams&) const = 0;
    virtual std::unique_ptr<pipeline::MergeStrategy> makeMerge()                      const = 0;
};

template <typename ConcreteT, typename ParamsT>
class ProcessingDescriptor : public ComponentDescriptor {
public:
    ProcessingDescriptor(ComponentType type, const char* name, Category cat)
        : type_(type), name_(name), cat_(cat) {}

    ComponentType type()        const override { return type_; }
    const char*   displayName() const override { return name_; }
    Category      category()    const override { return cat_;  }

    bool isMerge()   const override { return false; }
    bool hasParams() const override { return true; }

    NodeParams defaultParams() const override { return ParamsT{}; }

    std::unique_ptr<components::Component> makeComponent(const NodeParams& params) const override {
        return std::make_unique<ConcreteT>(std::get<ParamsT>(params));
    }

    std::unique_ptr<pipeline::MergeStrategy> makeMerge() const override {
        throw std::logic_error{ "ProcessingDescriptor::makeMerge called on a processing descriptor" };
    }

private:
    ComponentType type_;
    const char*   name_;
    Category      cat_;
};

template <typename MergeT>
class MergeDescriptor : public ComponentDescriptor {
public:
    MergeDescriptor(ComponentType type, const char* name)
        : type_(type), name_(name) {}

    ComponentType type()        const override { return type_; }
    const char*   displayName() const override { return name_; }
    Category      category()    const override { return Category::Merge; }

    bool isMerge()   const override { return true; }
    bool hasParams() const override { return false; }

    NodeParams defaultParams() const override {
        throw std::logic_error{ "MergeDescriptor::defaultParams called on a merge descriptor" };
    }

    std::unique_ptr<components::Component> makeComponent(const NodeParams&) const override {
        throw std::logic_error{ "MergeDescriptor::makeComponent called on a merge descriptor" };
    }

    std::unique_ptr<pipeline::MergeStrategy> makeMerge() const override {
        return std::make_unique<MergeT>();
    }

private:
    ComponentType type_;
    const char*   name_;
};

class ComponentRegistry {
public:
    ComponentRegistry() = delete;

    using DescriptorList = std::array<
        std::unique_ptr<ComponentDescriptor>,
        static_cast<size_t>(ComponentType::ComponentTypeCount)
    >;

    static const DescriptorList& all() {
        static const DescriptorList entries{ buildRegistry() };
        return entries;
    }

    static const ComponentDescriptor& get(ComponentType type) {
        if (static_cast<size_t>(type) < all().size()) {
            const std::unique_ptr<ComponentDescriptor>& desc{ all()[static_cast<size_t>(type)] };
            if (desc) {
                return *desc;
            }
        }
        throw std::invalid_argument{ "ComponentRegistry::get: unknown ComponentType" };
    }

    static std::vector<const ComponentDescriptor*> byCategory(Category cat) {
        std::vector<const ComponentDescriptor*> result;
        for (const std::unique_ptr<ComponentDescriptor>& d : all()) {
            if (d->category() == cat) {
                result.push_back(d.get());
            }
        }
        return result;
    }

    static const char* categoryName(Category cat) {
        switch (cat) {
            case Category::Denoising:       return "Denoising";
            case Category::EdgeDetection:   return "Edge Detection";
            case Category::ShapeDetection:  return "Shape Detection";
            case Category::Merge:           return "Merge Strategy";
            default:                        return "Unknown";
        }
    }

private:
    static DescriptorList buildRegistry() {
        using namespace components::denoising;
        using namespace components::edge_detection;
        using namespace components::shape_detection;

        DescriptorList descriptorList;

        initializeDescriptorList<TVDenoisingCPU, TVParams>(
            descriptorList, ComponentType::TVDenoisingCPU, "TV Denoising (CPU)", Category::Denoising
        );

        initializeDescriptorList<TVDenoisingGPU, TVParams>(
            descriptorList, ComponentType::TVDenoisingGPU, "TV Denoising (GPU)", Category::Denoising
        );

        initializeDescriptorList<GaussianBlurCPU, GaussParams>(
            descriptorList, ComponentType::GaussianBlurCPU, "Gaussian Blur (CPU)", Category::Denoising
        );

        initializeDescriptorList<GaussianBlurGPU, GaussParams>(
            descriptorList, ComponentType::GaussianBlurGPU, "Gaussian Blur (GPU)", Category::Denoising
        );

        initializeDescriptorList<SobelEdgeDetectionCPU, SobelParams>(
            descriptorList, ComponentType::SobelCPU, "Sobel Edge Detection (CPU)", Category::EdgeDetection
        );

        initializeDescriptorList<SobelEdgeDetectionGPU, SobelParams>(
            descriptorList, ComponentType::SobelGPU, "Sobel Edge Detection (GPU)", Category::EdgeDetection
        );

        initializeDescriptorList<CannyEdgeDetectionCPU, CannyParams>(
            descriptorList, ComponentType::CannyCPU, "Canny Edge Detection (CPU)", Category::EdgeDetection
        );

        initializeDescriptorList<CannyEdgeDetectionGPU, CannyParams>(
            descriptorList, ComponentType::CannyGPU, "Canny Edge Detection (GPU)", Category::EdgeDetection
        );

        initializeDescriptorList<HoughLineShapeDetectionCPU, HoughLParams>(
            descriptorList, ComponentType::HoughLineCPU, "Hough Line Detection (CPU)", Category::ShapeDetection
        );

        initializeDescriptorList<HoughLineShapeDetectionGPU, HoughLParams>(
            descriptorList, ComponentType::HoughLineGPU, "Hough Line Detection (GPU)", Category::ShapeDetection
        );

        initializeDescriptorList<HoughCircleShapeDetectionCPU, HoughCParams>(
            descriptorList, ComponentType::HoughCircleCPU, "Hough Circle Detection (CPU)", Category::ShapeDetection
        );

        initializeDescriptorList<HoughCircleShapeDetectionGPU, HoughCParams>(
            descriptorList, ComponentType::HoughCircleGPU, "Hough Circle Detection (GPU)", Category::ShapeDetection
        );

        initializeDescriptorList<pipeline::CombineEdgeMap>(
            descriptorList, ComponentType::CombineEdgeMap, "Combine Edge Map"
        );

        initializeDescriptorList<pipeline::CombineShapeMap>(
            descriptorList, ComponentType::CombineShapeMap, "Combine Shape Map"
        );

        return descriptorList;
    }

    template <typename ComponentT, typename ParamsT>
    static void initializeDescriptorList(DescriptorList& list, ComponentType type, const char* name, Category cat) {
        list[static_cast<size_t>(type)] = std::make_unique<ProcessingDescriptor<ComponentT, ParamsT>>(type, name, cat);
    }

    template <typename MergeT>
    static void initializeDescriptorList(DescriptorList& list, ComponentType type, const char* name) {
        list[static_cast<size_t>(type)] = std::make_unique<MergeDescriptor<MergeT>>(type, name);
    }
};

#endif // COMPONENT_REGISTRY_H
