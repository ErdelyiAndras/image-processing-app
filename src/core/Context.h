#ifndef CONTEXT_H
#define CONTEXT_H

#include "Image.h"

#include <string>

namespace components {
    class Context {
    public:
        Context(Image& image)
            : originalImage(image)
            , processedImage(image)
            , edgeMap(image.getRows(), image.getCols())
            , shapeMap(image.getRows(), image.getCols())
            , appliedComponents() {}

        inline const Image& getOriginalImage() const { return originalImage; }

        inline const Image& getProcessedImage() const { return processedImage; }
        inline Image& getProcessedImage() { return processedImage; }

        inline const Image& getEdgeMap() const { return edgeMap; }
        inline Image& getEdgeMap() { return edgeMap; }

        inline const Image& getShapeMap() const { return shapeMap; }
        inline Image& getShapeMap() { return shapeMap; }

        inline const std::string& getAppliedComponents() const { return appliedComponents; }
        inline void applyComponent(const std::string& componentName) { appliedComponents += "_" + componentName; }

    private:
        const Image originalImage;
        Image processedImage;
        Image edgeMap;
        Image shapeMap;

        std::string appliedComponents;
    };
} // components

#endif // CONTEXT_H
