#ifndef MERGE_STRATEGY_H
#define MERGE_STRATEGY_H

#include "Context.h"

#include <vector>
#include <string>

namespace pipeline {
    class MergeStrategy {
    public:
        virtual ~MergeStrategy() = default;

        components::Context merge(const std::vector<components::Context>& contexts) const {
            components::Context mergedContext{ contexts.front() };
            std::string mergedAppliedComponents{ "[" + mergedContext.getAppliedComponents() };
            for (size_t i{ 1U }; i < contexts.size(); ++i) {
                const components::Context& incomingContext{ contexts[i] };
                mergedAppliedComponents += "&" + incomingContext.getAppliedComponents();
                updateMergedContext(mergedContext, incomingContext);
            }
            mergedAppliedComponents += "]";
            mergedContext.setAppliedComponents(mergedAppliedComponents);
            mergedContext.applyComponent(getName());
            return mergedContext;
        }

    protected:
        virtual void updateMergedContext(components::Context& mergedContext, const components::Context& incomingContext) const = 0;
        virtual std::string getName() const = 0;
    };
}

#endif // MERGE_STRATEGY_H
