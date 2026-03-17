#ifndef COMPONENT_H
#define COMPONENT_H

#include "Context.h"
#include "Parameters.h"
#include "types.h"

#include <string>

namespace components {
    class Component {
    public:
        Component();
        virtual ~Component() = default;

        virtual void setParameters(const Parameters& params) = 0;
        virtual void process(Context& context);
        virtual std::string getName() const = 0;

    protected:
        PixelIdx height;
        PixelIdx width;
        Image inputImage;
        Image outputImage;

        virtual void processContext(const Context& context);
    };
} // components

#endif // COMPONENT_H
