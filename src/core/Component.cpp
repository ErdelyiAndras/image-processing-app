#include "Component.h"

namespace components {
    Component::Component()
        : height(0U)
        , width(0U)
        , inputImage()
        , outputImage() {}

    void Component::process(Context& context) {
        processContext(context);
        context.applyComponent(getName());
    }

    void Component::processContext(const Context& context) {
        inputImage  = context.getProcessedImage();
        outputImage = Image{ inputImage.getRows(), inputImage.getCols() };

        height = inputImage.getRows();
        width  = inputImage.getCols();
    }
} // components
