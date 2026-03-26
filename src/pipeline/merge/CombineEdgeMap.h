#ifndef COMBINE_EDGE_MAP_H
#define COMBINE_EDGE_MAP_H

#include "MergeStrategy.h"
#include "Context.h"
#include "Image.h"
#include "types.h"

namespace pipeline {
    class CombineEdgeMap final : public MergeStrategy {
    public:
        void updateMergedContext(components::Context& mergedContext, const components::Context& incomingContext) const override {
            Image& mergedEdgeMap{ mergedContext.getEdgeMap() };
            const Image& incomingEdgeMap{ incomingContext.getEdgeMap() };

            for (PixelIdx i{ 0U }; i < mergedEdgeMap.getRows(); ++i) {
                for (PixelIdx j{ 0U }; j < mergedEdgeMap.getCols(); ++j) {
                    if (incomingEdgeMap(i, j) != 0) {
                        mergedEdgeMap(i, j) = incomingEdgeMap(i, j);
                    }
                }
            }
        }

    private:
        std::string getName() const override {
            return "combine-edge-map";
        }
    };
}

#endif // COMBINE_EDGE_MAP_H
