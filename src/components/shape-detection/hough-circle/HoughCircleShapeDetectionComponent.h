#ifndef HOUGH_CIRCLE_SHAPE_DETECTION_COMPONENT_H
#define HOUGH_CIRCLE_SHAPE_DETECTION_COMPONENT_H

#include "ShapeDetectionComponent.h"
#include "HoughCircleShapeDetectionParameters.h"
#include "Context.h"
#include "types.h"
#include "HoughCircle.h"

#include <vector>

namespace components {
    namespace shape_detection {
        class HoughCircleShapeDetectionComponent : public ShapeDetectionComponent {
        public:
            HoughCircleShapeDetectionComponent();
            HoughCircleShapeDetectionComponent(
                uint32_t vote_min_threshold,
                uint32_t min_radius,
                uint32_t max_radius,
                float min_dist
            );

            virtual ~HoughCircleShapeDetectionComponent() = default;

            inline uint32_t getVoteMinThreshold() const { return vote_min_threshold; }
            inline uint32_t getMinRadius() const { return min_radius; }
            inline uint32_t getMaxRadius() const { return max_radius; }
            inline float getMinDist() const { return min_dist; }

            void setParameters(const Parameters& params) override final;

        protected:
            uint32_t vote_min_threshold;
            uint32_t min_radius;
            uint32_t max_radius;
            float min_dist;

            std::vector<HoughCircle> detected_circles;

            virtual void applyHoughTransform() = 0;
            void processDetectedCircles();

            virtual void processContext(const Context& context) override;

        private:
            using ParamType = HoughCircleShapeDetectionParameters;

            void applyShapeDetection() override final;

            void plot(PixelIdx x, PixelIdx y);
        };
    } // shape_detection
} // components

#endif // HOUGH_CIRCLE_SHAPE_DETECTION_COMPONENT_H
