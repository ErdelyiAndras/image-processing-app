#ifndef HOUGH_LINE_SHAPE_DETECTION_COMPONENT_H
#define HOUGH_LINE_SHAPE_DETECTION_COMPONENT_H

#include "ShapeDetectionComponent.h"
#include "HoughLineShapeDetectionParameters.h"
#include "HoughLine.h"
#include "types.h"

#include <cstdint>
#include <utility>
#include <vector>

namespace components {
    namespace shape_detection {
        class HoughLineShapeDetectionComponent : public ShapeDetectionComponent<HoughLineShapeDetectionParameters> {
        public:
            explicit HoughLineShapeDetectionComponent(const ParamType& params);

            virtual ~HoughLineShapeDetectionComponent() = default;

            inline float getRhoResolution()       const { return parameters.rho_resolution; }
            inline float getThetaResolution()     const { return parameters.theta_resolution; }
            inline uint32_t getVoteMinThreshold() const { return parameters.vote_min_threshold; }
            inline uint32_t getMinLineLength()    const { return parameters.min_line_length; }
            inline uint32_t getMaxLineGap()       const { return parameters.max_line_gap; }

            void setParameters(const Parameters& params) override final;

        protected:
            uint32_t num_rho_bins;
            uint32_t num_theta_bins;
            float rho_max;

            std::vector<HoughLine> detected_lines;
            std::vector<uint32_t> accumulator;

            std::vector<float> cos_table;
            std::vector<float> sin_table;

            virtual void applyHoughTransform() = 0;
            void nonMaximumSuppression();
            void processDetectedLines();

            virtual void processContext(const Context& context) override;

        private:
            using Pixel     = std::pair<PixelIdx, PixelIdx>;

            void applyShapeDetection() override final;
        };
    } // shape_detection
} // components

#endif // HOUGH_LINE_SHAPE_DETECTION_COMPONENT_H
