#include "HoughLineShapeDetectionComponent.h"
#include "types.h"
#include "config.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

namespace components {
    namespace shape_detection {
        HoughLineShapeDetectionComponent::HoughLineShapeDetectionComponent()
            : ShapeDetectionComponent()
            , rho_resolution(default_rho_resolution)
            , theta_resolution(default_theta_resolution)
            , vote_min_threshold(default_threshold)
            , min_line_length(default_min_line_length)
            , max_line_gap(default_max_line_gap)
            , num_rho_bins(0U)
            , num_theta_bins(0U)
            , rho_max(0.0f)
            , detected_lines() {}

        HoughLineShapeDetectionComponent::HoughLineShapeDetectionComponent(
            float rho_res, float theta_res, uint32_t vote_min_threshold, uint32_t min_line_length, uint32_t max_line_gap
        )
            : ShapeDetectionComponent()
            , rho_resolution(rho_res)
            , theta_resolution(theta_res)
            , vote_min_threshold(vote_min_threshold)
            , min_line_length(min_line_length)
            , max_line_gap(max_line_gap)
            , num_rho_bins(0U)
            , num_theta_bins(0U)
            , rho_max(0.0f)
            , detected_lines() {}

        void HoughLineShapeDetectionComponent::setParameters(const Parameters& params) {
            ParamType shapeDetectionParams{ dynamic_cast<const ParamType&>(params) };
            rho_resolution     = shapeDetectionParams.rho_resolution;
            theta_resolution   = shapeDetectionParams.theta_resolution;
            vote_min_threshold = shapeDetectionParams.vote_min_threshold;
            min_line_length    = shapeDetectionParams.min_line_length;
            max_line_gap       = shapeDetectionParams.max_line_gap;
        }

        void HoughLineShapeDetectionComponent::processDetectedLines() {
            std::sort(detected_lines.begin(), detected_lines.end(),
                [](const HoughLine& a, const HoughLine& b) {
                    return a.votes > b.votes;
                }
            );

            std::vector<HoughLine> valid_lines;

            for (HoughLine& line : detected_lines) {
                const float cos_theta{ std::cos(line.theta) };
                const float sin_theta{ std::sin(line.theta) };

                std::vector<Pixel> line_pixels;
                // horizontal-ish line
                if (std::abs(sin_theta) > std::abs(cos_theta)) {
                    for (PixelIdx x{ 0U }; x < width; ++x) {
                        const float y_float{ std::round((line.rho - static_cast<float>(x) * cos_theta) / sin_theta) };
                        if (0 <= y_float && y_float < height) {
                            line_pixels.emplace_back(Pixel{ x, static_cast<PixelIdx>(y_float) });
                        }
                    }
                }
                // vertical-ish line
                else {
                    for (PixelIdx y{ 0U }; y < height; ++y) {
                        const float x_float{ std::round((line.rho - static_cast<float>(y) * sin_theta) / cos_theta) };
                        if (0 <= x_float && x_float < width) {
                            line_pixels.emplace_back(Pixel{ static_cast<PixelIdx>(x_float), y });
                        }
                    }
                }

                int run_start{ -1 };
                uint32_t gap_count{ 0U };
                int best_run_start{ -1 };
                int best_run_end{ -1 };
                uint32_t best_run_length{ 0U };

                for (uint32_t k{ 0U }; k < line_pixels.size(); ++k) {
                    const Pixel& pixel{ line_pixels[k] };
                    const bool on_edge{ inputImage(pixel.second, pixel.first) > 0.0f };

                    if (on_edge) {
                        if (run_start == -1) {
                            run_start = static_cast<int>(k);
                        }
                        gap_count = 0U;
                    }
                    else if (run_start != -1) {
                        ++gap_count;
                        if (gap_count > max_line_gap) {
                            const int run_length{ static_cast<int>(k) - run_start - static_cast<int>(gap_count) };
                            if (run_length > static_cast<int>(best_run_length)) {
                                best_run_start  = run_start;
                                best_run_end    = static_cast<int>(k) - static_cast<int>(gap_count) - 1;
                                best_run_length = static_cast<uint32_t>(run_length);
                            }
                            run_start = -1;
                            gap_count = 0U;
                        }
                    }
                }

                if (run_start != -1) {
                    const int run_length{ static_cast<int>(line_pixels.size()) - run_start - static_cast<int>(gap_count) };
                    if (run_length > static_cast<int>(best_run_length)) {
                        best_run_start  = run_start;
                        best_run_end    = static_cast<int>(line_pixels.size()) - static_cast<int>(gap_count) - 1;
                        best_run_length = static_cast<uint32_t>(run_length);
                    }
                }

                if (best_run_length < min_line_length) {
                    continue;
                }

                line.x0 = line_pixels[static_cast<size_t>(best_run_start)].first;
                line.y0 = line_pixels[static_cast<size_t>(best_run_start)].second;
                line.x1 = line_pixels[static_cast<size_t>(best_run_end)].first;
                line.y1 = line_pixels[static_cast<size_t>(best_run_end)].second;

                int x0{ static_cast<int>(line.x0) };
                int y0{ static_cast<int>(line.y0) };
                const int x1{ static_cast<int>(line.x1) };
                const int y1{ static_cast<int>(line.y1) };

                const int dx{  std::abs(x1 - x0) };
                const int dy{ -std::abs(y1 - y0) };
                const int sx{ x0 < x1 ? 1 : -1 };
                const int sy{ y0 < y1 ? 1 : -1 };
                int err{ dx + dy };

                while (true) {
                    if (x0 >= 0 && x0 < static_cast<int>(width) &&
                        y0 >= 0 && y0 < static_cast<int>(height)) {
                        outputImage(static_cast<PixelIdx>(y0), static_cast<PixelIdx>(x0)) = 1.0f;
                    }
                    if (x0 == x1 && y0 == y1) {
                        break;
                    }
                    const int e2{ 2 * err };
                    if (e2 >= dy) {
                        err += dy;
                        x0 += sx;
                    }
                    if (e2 <= dx) {
                        err += dx;
                        y0 += sy;
                    }
                }

                valid_lines.push_back(line);
            }

            detected_lines = std::move(valid_lines);
        }

        void HoughLineShapeDetectionComponent::processContext(const Context& context) {
            ShapeDetectionComponent::processContext(context);
            rho_max        = std::sqrt(static_cast<float>(height * height + width * width));
            num_rho_bins   = static_cast<uint32_t>(std::ceil(2 * rho_max / rho_resolution)) + 1U;
            num_theta_bins = static_cast<uint32_t>(std::ceil(pi / theta_resolution));
            detected_lines.clear();
        }

        void HoughLineShapeDetectionComponent::applyShapeDetection() {
            applyHoughTransform();
            processDetectedLines();
        }
    } // shape_detection
} // components
