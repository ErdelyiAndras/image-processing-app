#ifndef PARAMETERIZED_COMPONENT_H
#define PARAMETERIZED_COMPONENT_H

#include "Component.h"
#include "Parameters.h"

#include <typeinfo>

namespace components {
    template <typename T>
    class ParameterizedComponent : public Component {
    public:
        using ParamType = T;

        explicit ParameterizedComponent(const ParamType& params)
            : parameters(params) {}

        virtual ~ParameterizedComponent() = default;

        virtual void setParameters(const Parameters& params) override {
            const ParamType* typedParams{ dynamic_cast<const ParamType*>(&params) };
            if (!typedParams) {
                throw std::bad_cast{};
            }
            parameters = *typedParams;
        }

    protected:
        ParamType parameters;
    };
} // components

#endif // PARAMETERIZED_COMPONENT_H
