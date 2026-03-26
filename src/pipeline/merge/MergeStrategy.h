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
            std::string mergedTrace{ "(" + mergedContext.getAppliedComponents() };
            for (size_t i{ 1U }; i < contexts.size(); ++i) {
                const components::Context& incomingContext{ contexts[i] };
                mergedTrace += "+" + incomingContext.getAppliedComponents();
                updateMergedContext(mergedContext, incomingContext);
            }
            mergedTrace += ")";
            mergedContext.setAppliedComponents({ mergedTrace, getName() });
            return mergedContext;
        }

        virtual std::string getName() const = 0;

    protected:
        virtual void updateMergedContext(components::Context& mergedContext, const components::Context& incomingContext) const = 0;
    };
}

#endif // MERGE_STRATEGY_H
