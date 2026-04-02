#ifndef CONTEXT_H
#define CONTEXT_H

#include "Image.h"
#include "Color.h"

#include <string>
#include <vector>
#include <memory>

namespace components {
    class Context {
    public:
        Context(Image& image)
            : originalImage(std::make_shared<const Image>(image))
            , processedImage(image)
            , edgeMap(image.getRows(), image.getCols())
            , shapeMap(image.getRows(), image.getCols())
            , appliedComponents() {}

        inline const Image& getOriginalImage() const { return *originalImage; }

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
            return Image::saveComposite(
                base,
                ext,
                getProcessedImage(),
                {
                    { &getEdgeMap(), Color::Blue },
                    { &getShapeMap(), Color::Red }
                }
            );
        };

    private:
        std::shared_ptr<const Image> originalImage;
        Image processedImage;
        Image edgeMap;
        Image shapeMap;

        std::vector<std::string> appliedComponents;
    };
} // components

#endif // CONTEXT_H
