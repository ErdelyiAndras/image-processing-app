#ifndef CANNY_EDGE_DETECTION_GPU_H
#define CANNY_EDGE_DETECTION_GPU_H

#include "CannyEdgeDetectionComponent.h"
#include "GPUComponent.h"

#include <CL/opencl.hpp>
#include <string>

namespace components {
    namespace edge_detection {
        class CannyEdgeDetectionGPU final : public CannyEdgeDetectionComponent, protected GPUComponent {
        public:
            CannyEdgeDetectionGPU();
            explicit CannyEdgeDetectionGPU(const ParamType& params);
            explicit CannyEdgeDetectionGPU(float low_threshold, float high_threshold);

            inline std::string getName() const override { return "canny-gpu"; }

        private:
            cl::Buffer grad_mag_buffer;
            cl::Buffer grad_dir_buffer;
            cl::Buffer nms_buffer;
            cl::Buffer output_buffer;
            cl::Buffer temp_out_buffer;

            void calculateSobelGradient() override;
            void nonMaximumSuppression() override;
            void doubleThresholding() override;
            void edgeTrackingByHysteresis() override;

            void processContext(const Context& context) override;
        };
    } // edge_detection
} // components

#endif // CANNY_EDGE_DETECTION_GPU_H
