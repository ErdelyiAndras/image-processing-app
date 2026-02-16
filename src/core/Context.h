#ifndef CONTEXT_H
#define CONTEXT_H

#include "Image.h"

class Context {
public:
    Context(const Image& image) : originalImage(image), processedImage(image) {}

    const Image& getOriginalImage() const { return originalImage; }
    Image& getProcessedImage() { return processedImage; }

private:
    Image originalImage;
    Image processedImage;
};

#endif // CONTEXT_H
