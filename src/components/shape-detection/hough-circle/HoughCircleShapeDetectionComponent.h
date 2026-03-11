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
                float    min_dist,
                uint32_t num_angle_steps
            );

            virtual ~HoughCircleShapeDetectionComponent() = default;

            inline uint32_t getVoteMinThreshold() const { return vote_min_threshold; }
            inline uint32_t getMinRadius() const { return min_radius; }
            inline uint32_t getMaxRadius() const { return max_radius; }
            inline float    getMinDist() const { return min_dist; }
            inline uint32_t getNumAngleSteps() const { return num_angle_steps; }

            void setParameters(const Parameters& params) override final;

        protected:
            uint32_t vote_min_threshold;
            uint32_t min_radius;
            uint32_t max_radius;
            float    min_dist;
            uint32_t num_angle_steps;

            uint32_t num_radii;

            std::vector<HoughCircle> detected_circles;
            std::vector<uint32_t> accumulator;

            std::vector<float> cos_table;
            std::vector<float> sin_table;

            virtual void applyHoughTransform() = 0;
            void nonMaximumSuppression();
            void processDetectedCircles();

            virtual void processContext(const Context& context) override;

        private:
            using ParamType = HoughCircleShapeDetectionParameters;

            void applyShapeDetection() override final;

            void plot(int x, int y);
        };
    } // shape_detection
} // components

#endif // HOUGH_CIRCLE_SHAPE_DETECTION_COMPONENT_H
