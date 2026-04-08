#include "SobelEdgeDetectionCPU.h"

#include "Image.h"
#include "SobelEdgeDetectionComponent.h"
#include "types.h"

#include <algorithm>
#include <cmath>

namespace components::edge_detection {
    SobelEdgeDetectionCPU::SobelEdgeDetectionCPU()
        : SobelEdgeDetectionCPU(ParamType{}) {}

    SobelEdgeDetectionCPU::SobelEdgeDetectionCPU(const ParamType& params)
        : SobelEdgeDetectionComponent(params) {}

    SobelEdgeDetectionCPU::SobelEdgeDetectionCPU(float threshold)
        : SobelEdgeDetectionCPU(ParamType{ threshold }) {}

    void SobelEdgeDetectionCPU::applySobelFilter() {
        // Sobel kernels:
        // Gx = [[-1, 0, 1], [-2, 0, 2], [-1, 0, 1]]
        // Gy = [[-1, -2, -1], [0, 0, 0], [1, 2, 1]]

        for (PixelIdx i{ 1U }; i < height - 1; ++i) {
            for (PixelIdx j{ 1U }; j < width - 1; ++j) {
                const float gx{
                    - (1.0f * inputImage(i - 1, j - 1)) + (1.0f * inputImage(i - 1, j + 1))
                    - (2.0f * inputImage(i,     j - 1)) + (2.0f * inputImage(i,     j + 1))
                    - (1.0f * inputImage(i + 1, j - 1)) + (1.0f * inputImage(i + 1, j + 1))
                };

                const float gy{
                    - (1.0f * inputImage(i - 1, j - 1)) - (2.0f * inputImage(i - 1, j)) - (1.0f * inputImage(i - 1, j + 1))
                    + (1.0f * inputImage(i + 1, j - 1)) + (2.0f * inputImage(i + 1, j)) + (1.0f * inputImage(i + 1, j + 1))
                };

                outputImage(i, j) = std::sqrt((gx * gx) + (gy * gy));
            }
        }

        for (PixelIdx i{ 0U }; i < height; ++i) {
            for (PixelIdx j{ 0U }; j < width; ++j) {
                if (outputImage(i, j) < parameters.threshold) {
                    outputImage(i, j) = 0.0f;
                }
                outputImage(i, j) = std::min(outputImage(i, j), 1.0f);
            }
        }
    }
} // namespace components::edge_detection
