#include "CannyEdgeDetectionCPU.h"
#include "CannyEdgeDetectionParameters.h"

#include "types.h"
#include "config.h"

#include <cmath>
#include <iostream>

namespace components {
    namespace edge_detection {
        CannyEdgeDetectionCPU::CannyEdgeDetectionCPU()
            : CannyEdgeDetectionCPU(CannyEdgeDetectionParameters{}) {}

        CannyEdgeDetectionCPU::CannyEdgeDetectionCPU(const CannyEdgeDetectionParameters& params)
            : CannyEdgeDetectionComponent(params)
            , grad_mag()
            , grad_dir()
            , nms() {}

        CannyEdgeDetectionCPU::CannyEdgeDetectionCPU(float low_threshold, float high_threshold)
            : CannyEdgeDetectionCPU(CannyEdgeDetectionParameters{ low_threshold, high_threshold }) {}

        void CannyEdgeDetectionCPU::calculateSobelGradient() {
            for (PixelIdx i{ 1U }; i < height - 1; ++i) {
                for (PixelIdx j{ 1U }; j < width - 1; ++j) {
                    const float gx{
                        -1.0f * inputImage(i - 1, j - 1) + 1.0f * inputImage(i - 1, j + 1)
                        -2.0f * inputImage(i,     j - 1) + 2.0f * inputImage(i,     j + 1)
                        -1.0f * inputImage(i + 1, j - 1) + 1.0f * inputImage(i + 1, j + 1)
                    };

                    const float gy{
                        -1.0f * inputImage(i - 1, j - 1) - 2.0f * inputImage(i - 1, j) - 1.0f * inputImage(i - 1, j + 1)
                        +1.0f * inputImage(i + 1, j - 1) + 2.0f * inputImage(i + 1, j) + 1.0f * inputImage(i + 1, j + 1)
                    };

                    grad_mag(i, j) = std::sqrt(gx * gx + gy * gy);
                    grad_dir(i, j) = std::atan2(gy, gx);
                }
            }
        }

        void CannyEdgeDetectionCPU::nonMaximumSuppression() {
            for (PixelIdx i{ 1U }; i < height - 1; ++i) {
                for (PixelIdx j{ 1U }; j < width - 1; ++j) {
                    float angle{ grad_dir(i, j) * 180.0f / pi };
                    if (angle < 0.0f) {
                        angle += 180.0f;
                    }

                    float q{ 0.0f };
                    float r{ 0.0f };

                    // 0 degrees (horizontal)
                    if ((angle >= 0.0f && angle < 22.5f) || (angle >= 157.5f && angle <= 180.0f)) {
                        q = grad_mag(i, j + 1);
                        r = grad_mag(i, j - 1);
                    }
                    // 45 degrees
                    else if (angle >= 22.5f && angle < 67.5f) {
                        q = grad_mag(i + 1, j - 1);
                        r = grad_mag(i - 1, j + 1);
                    }
                    // 90 degrees (vertical)
                    else if (angle >= 67.5f && angle < 112.5f) {
                        q = grad_mag(i + 1, j);
                        r = grad_mag(i - 1, j);
                    }
                    // 135 degrees
                    else if (angle >= 112.5f && angle < 157.5f) {
                        q = grad_mag(i - 1, j - 1);
                        r = grad_mag(i + 1, j + 1);
                    }

                    if (grad_mag(i, j) >= q && grad_mag(i, j) >= r) {
                        nms(i, j) = grad_mag(i, j);
                    }
                }
            }
        }

        void CannyEdgeDetectionCPU::doubleThresholding() {
            for (PixelIdx i{ 0U }; i < height; ++i) {
                for (PixelIdx j{ 0U }; j < width; ++j) {
                    if (nms(i, j) >= high_threshold) {
                        outputImage(i, j) = strong;
                    } else if (nms(i, j) >= low_threshold) {
                        outputImage(i, j) = weak;
                    } else {
                        outputImage(i, j) = 0.0f;
                    }
                }
            }
        }

        void CannyEdgeDetectionCPU::edgeTrackingByHysteresis() {
            uint32_t counter{ 1U };
            while (changed) {
                changed = false;
                for (PixelIdx i{ 1U }; i < height - 1; ++i) {
                    for (PixelIdx j{ 1U }; j < width - 1; ++j) {
                        if (outputImage(i, j) == weak) {
                            if (outputImage(i - 1, j - 1) == strong || outputImage(i - 1, j    ) == strong ||
                                outputImage(i - 1, j + 1) == strong || outputImage(i,     j - 1) == strong ||
                                outputImage(i,     j + 1) == strong || outputImage(i + 1, j - 1) == strong ||
                                outputImage(i + 1, j    ) == strong || outputImage(i + 1, j + 1) == strong) {
                                outputImage(i, j) = strong;
                                changed = true;
                            }
                        }
                    }
                }
                if (ENABLE_LOGGING) {
                    std::cout << "Hysteresis iteration " << counter << ", changed: " << changed << std::endl;
                }
                ++counter;
            }

            for (PixelIdx i{ 0U }; i < height; ++i) {
                for (PixelIdx j{ 0U }; j < width; ++j) {
                    if (outputImage(i, j) != strong) {
                        outputImage(i, j) = 0.0f;
                    }
                }
            }
        }

        void CannyEdgeDetectionCPU::processContext(const Context& context) {
            CannyEdgeDetectionComponent::processContext(context);
            grad_mag = Image{ height, width };
            grad_dir = Image{ height, width };
            nms      = Image{ height, width };
        }
    } // edge_detection
} // components
