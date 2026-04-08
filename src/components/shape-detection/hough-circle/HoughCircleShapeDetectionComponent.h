#ifndef HOUGH_CIRCLE_SHAPE_DETECTION_COMPONENT_H
#define HOUGH_CIRCLE_SHAPE_DETECTION_COMPONENT_H

#include "HoughCircle.h"
#include "HoughCircleShapeDetectionParameters.h"
#include "ShapeDetectionComponent.h"
#include "types.h"

#include <cstdint>
#include <vector>

namespace components {
    namespace shape_detection {
        class HoughCircleShapeDetectionComponent : public ShapeDetectionComponent<HoughCircleShapeDetectionParameters> {
        public:
            explicit HoughCircleShapeDetectionComponent(const ParamType& params);

            virtual ~HoughCircleShapeDetectionComponent() = default;

            inline int   getVoteMinThreshold() const { return parameters.vote_min_threshold; }
            inline int   getMinRadius()        const { return parameters.min_radius; }
            inline int   getMaxRadius()        const { return parameters.max_radius; }
            inline float getMinDist()          const { return parameters.min_dist; }
            inline int   getNumAngleSteps()    const { return parameters.num_angle_steps; }

            void setParameters(const Parameters& params) override final;

        protected:
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
            void applyShapeDetection() override final;

            bool isCircleTooClose(const HoughCircle& circle, const std::vector<HoughCircle>& valid_circles) const;
            void drawCircle(const HoughCircle& circle);

            bool isLocalMax(uint32_t r_idx, PixelIdx cx, PixelIdx cy, uint32_t votes) const;
            void plot(int x, int y);
        };
    } // shape_detection
} // components

#endif // HOUGH_CIRCLE_SHAPE_DETECTION_COMPONENT_H
