#ifndef COMBINE_EDGE_MAP_H
#define COMBINE_EDGE_MAP_H

#include "Context.h"
#include "Image.h"
#include "MergeStrategy.h"
#include "types.h"

#include <algorithm>

namespace pipeline {
    class CombineEdgeMap final : public MergeStrategy {
    public:
        std::string getName() const override {
            return "combine-edge-map";
        }

    private:
        void updateMergedContext(components::Context& mergedContext, const components::Context& incomingContext) const override {
            Image& mergedEdgeMap{ mergedContext.getEdgeMap() };
            const Image& incomingEdgeMap{ incomingContext.getEdgeMap() };

            for (PixelIdx i{ 0U }; i < mergedEdgeMap.getRows(); ++i) {
                for (PixelIdx j{ 0U }; j < mergedEdgeMap.getCols(); ++j) {
                    mergedEdgeMap(i, j) = std::max(mergedEdgeMap(i, j), incomingEdgeMap(i, j));
                }
            }
        }
    };
}

#endif // COMBINE_EDGE_MAP_H
