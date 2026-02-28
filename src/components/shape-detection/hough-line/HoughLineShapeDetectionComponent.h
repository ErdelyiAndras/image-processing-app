#ifndef HOUGH_LINE_SHAPE_DETECTION_COMPONENT_H
#define HOUGH_LINE_SHAPE_DETECTION_COMPONENT_H

#include "shape-detection-config.h"
#include "ShapeDetectionComponent.h"
#include "HoughLineShapeDetectionParameters.h"
#include "HoughLine.h"
#include "Context.h"
#include "types.h"

#include <vector>
#include <utility>

namespace components {
    namespace shape_detection {
        class HoughLineShapeDetectionComponent : public ShapeDetectionComponent {
        public:
            HoughLineShapeDetectionComponent();
            HoughLineShapeDetectionComponent(
                float rho_resolution,
                float theta_resolution,
                uint32_t vote_min_threshold,
                uint32_t min_line_length,
                uint32_t max_line_gap
            );

            virtual ~HoughLineShapeDetectionComponent() = default;

            inline float getRhoResolution() const { return rho_resolution; }
            inline float getThetaResolution() const { return theta_resolution; }
            inline uint32_t getVoteMinThreshold() const { return vote_min_threshold; }
            inline uint32_t getMinLineLength() const { return min_line_length; }
            inline uint32_t getMaxLineGap() const { return max_line_gap; }

            void setParameters(const Parameters& params) override final;

        protected:
            float rho_resolution;
            float theta_resolution;
            uint32_t vote_min_threshold;
            uint32_t min_line_length;
            uint32_t max_line_gap;

            uint32_t num_rho_bins;
            uint32_t num_theta_bins;
            float rho_max;

            std::vector<HoughLine> detected_lines;

            virtual void applyHoughTransform() = 0;
            void processDetectedLines();

            virtual void processContext(const Context& context) override;

        private:
            using ParamType = HoughLineShapeDetectionParameters;
            using Pixel     = std::pair<PixelIdx, PixelIdx>;

            void applyShapeDetection() override final;
        };
    } // shape_detection
} // components

#endif // HOUGH_LINE_SHAPE_DETECTION_COMPONENT_H
