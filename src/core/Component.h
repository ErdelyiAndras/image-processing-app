#ifndef COMPONENT_H
#define COMPONENT_H

#include "Context.h"
#include "Parameters.h"

namespace components {
    class Component {
    public:
        virtual ~Component() = default;

        virtual void setParameters(const Parameters& params) = 0;
        virtual void process(Context& context) = 0;
    };
} // components

#endif // COMPONENT_H
