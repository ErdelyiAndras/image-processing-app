#ifndef COMBINE_SHAPE_MAP_H
#define COMBINE_SHAPE_MAP_H

#include "MergeStrategy.h"
#include "Context.h"
#include "Image.h"
#include "types.h"

namespace pipeline {
    class CombineShapeMap final : public MergeStrategy {
    public:
        void updateMergedContext(components::Context& mergedContext, const components::Context& incomingContext) const override {
            Image& mergedShapeMap{ mergedContext.getShapeMap() };
            const Image& incomingShapeMap{ incomingContext.getShapeMap() };

            for (PixelIdx i{ 0U }; i < mergedShapeMap.getRows(); ++i) {
                for (PixelIdx j{ 0U }; j < mergedShapeMap.getCols(); ++j) {
                    if (incomingShapeMap(i, j) != 0) {
                        mergedShapeMap(i, j) = incomingShapeMap(i, j);
                    }
                }
            }
        }
    private:
        std::string getName() const override {
            return "combine-shape-map";
        }
    };
}

#endif // COMBINE_SHAPE_MAP_H
