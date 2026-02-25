#ifndef CONTEXT_H
#define CONTEXT_H

#include "Image.h"

namespace components {
    class Context {
    public:
        Context(Image& image)
            : originalImage(image)
            , processedImage(image)
            , edgeImage(image.getRows(), image.getCols()) {}

        inline const Image& getOriginalImage() const { return originalImage; }

        inline const Image& getProcessedImage() const { return processedImage; }
        inline Image& getProcessedImage() { return processedImage; }

        inline const Image& getEdgeImage() const { return edgeImage; }
        inline Image& getEdgeImage() { return edgeImage; }
    private:
        const Image originalImage;
        Image processedImage;
        Image edgeImage;
    };
} // components

#endif // CONTEXT_H
