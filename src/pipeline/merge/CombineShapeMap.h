#ifndef COMBINE_SHAPE_MAP_H
#define COMBINE_SHAPE_MAP_H

#include "Context.h"
#include "Image.h"
#include "MergeStrategy.h"
#include "types.h"

#include <algorithm>

namespace pipeline {
    class CombineShapeMap final : public MergeStrategy {
    public:
        std::string getName() const override {
            return "combine-shape-map";
        }

    private:
        void updateMergedContext(components::Context& mergedContext, const components::Context& incomingContext) const override {
            Image& mergedShapeMap{ mergedContext.getShapeMap() };
            const Image& incomingShapeMap{ incomingContext.getShapeMap() };

            for (PixelIdx i{ 0U }; i < mergedShapeMap.getRows(); ++i) {
                for (PixelIdx j{ 0U }; j < mergedShapeMap.getCols(); ++j) {
                    mergedShapeMap(i, j) = std::max(mergedShapeMap(i, j), incomingShapeMap(i, j));
                }
            }
        }
    };
}

#endif // COMBINE_SHAPE_MAP_H
