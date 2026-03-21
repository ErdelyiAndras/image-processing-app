#ifndef CONTEXT_H
#define CONTEXT_H

#include "Image.h"
#include "Color.h"

#include <string>
#include <vector>

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

        std::string getAppliedComponents() const {
            std::string appliedComponentsStr;
            for (size_t i{ 0U }; i < appliedComponents.size(); ++i) {
                appliedComponentsStr += appliedComponents.at(i);
                if (i + 1U < appliedComponents.size()) {
                    appliedComponentsStr += "_";
                }
            }
            return appliedComponentsStr;
        }
        inline void applyComponent(const std::string& componentName) { appliedComponents.push_back(componentName); }
        inline void setAppliedComponents(std::vector<std::string> components) {
            appliedComponents = std::move(components);
        }

        bool save(const std::string& base, const std::string& ext) const {
            const std::string name{ base + "_output" + getAppliedComponents() };
            return Image::saveComposite(
                name,
                ext,
                getProcessedImage(),
                {
                    { &getEdgeMap(), Color::Blue },
                    { &getShapeMap(), Color::Red }
                }
            );
        };

    private:
        const Image originalImage;
        Image processedImage;
        Image edgeMap;
        Image shapeMap;

        std::vector<std::string> appliedComponents;
    };
} // components

#endif // CONTEXT_H
