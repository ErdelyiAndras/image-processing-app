#ifndef CONTEXT_H
#define CONTEXT_H

#include "Image.h"

namespace components {
    class Context {
    public:
        Context(Image& image) : originalImage(image), processedImage(image) {}

        const Image& getOriginalImage() const { return originalImage; }

        const Image& getProcessedImage() const { return processedImage; }
        Image& getProcessedImage() { return processedImage; }
    private:
        const Image originalImage;
        Image processedImage;
    };
} // components

#endif // CONTEXT_H
