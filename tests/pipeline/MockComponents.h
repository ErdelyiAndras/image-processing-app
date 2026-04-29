#ifndef MOCK_COMPONENTS_H
#define MOCK_COMPONENTS_H

#include "DenoisingComponent.h"
#include "EdgeDetectionComponent.h"
#include "ShapeDetectionComponent.h"
#include "Parameters.h"
#include "types.h"

namespace test {
    struct MockParams : public Parameters {};

    class MockDenoisingComponent : public components::denoising::DenoisingComponent<MockParams> {
    public:
        explicit MockDenoisingComponent(float fillValue = 0.0f)
            : DenoisingComponent<MockParams>(MockParams{})
            , fillValue(fillValue) {}

        std::string getName() const override { return "mock-denoising"; }

    private:
        float fillValue;

        void applyDenoising() override {
            for (PixelIdx i{ 0U }; i < this->height; ++i) {
                for (PixelIdx j{ 0U }; j < this->width; ++j) {
                    this->outputImage(i, j) = fillValue;
                }
            }
        }
    };

    class MockEdgeDetectionComponent : public components::edge_detection::EdgeDetectionComponent<MockParams> {
    public:
        explicit MockEdgeDetectionComponent(float fillValue = 0.0f)
            : EdgeDetectionComponent<MockParams>(MockParams{})
            , fillValue(fillValue) {}

        std::string getName() const override { return "mock-edge-detection"; }

    private:
        float fillValue;

        void applyEdgeDetection() override {
            for (PixelIdx i{ 0U }; i < this->height; ++i) {
                for (PixelIdx j{ 0U }; j < this->width; ++j) {
                    this->outputImage(i, j) = fillValue;
                }
            }
        }
    };

    class MockShapeDetectionComponent : public components::shape_detection::ShapeDetectionComponent<MockParams> {
    public:
        explicit MockShapeDetectionComponent(float fillValue = 0.0f)
            : ShapeDetectionComponent<MockParams>(MockParams{})
            , fillValue(fillValue) {}

        std::string getName() const override { return "mock-shape-detection"; }

    private:
        float fillValue;

        void applyShapeDetection() override {
            for (PixelIdx i{ 0U }; i < this->height; ++i) {
                for (PixelIdx j{ 0U }; j < this->width; ++j) {
                    this->outputImage(i, j) = fillValue;
                }
            }
        }
    };

} // namespace test

#endif // MOCK_COMPONENTS_H
