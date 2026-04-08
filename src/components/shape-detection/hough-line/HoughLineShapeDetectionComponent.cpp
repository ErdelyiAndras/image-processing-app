#include "HoughLineShapeDetectionComponent.h"

#include "Context.h"
#include "HoughLine.h"
#include "Image.h"
#include "Parameters.h"
#include "ShapeDetectionComponent.h"
#include "config.h"
#include "types.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <vector>

namespace components::shape_detection {
    HoughLineShapeDetectionComponent::HoughLineShapeDetectionComponent(
        const ParamType& params
    )
        : ShapeDetectionComponent(params)
        , num_rho_bins(0U)
        , num_theta_bins(static_cast<uint32_t>(std::ceil(pi / params.theta_resolution)))
        , rho_max(0.0f) {}

    void HoughLineShapeDetectionComponent::setParameters(const Parameters& params) {
        ShapeDetectionComponent::setParameters(params);
        num_theta_bins = static_cast<uint32_t>(std::ceil(pi / parameters.theta_resolution));
    }

    void HoughLineShapeDetectionComponent::nonMaximumSuppression() {
        for (uint32_t rho_idx{ 1U }; rho_idx < num_rho_bins - 1; ++rho_idx) {
            for (uint32_t theta_idx{ 1U }; theta_idx < num_theta_bins - 1; ++theta_idx) {
                const uint32_t acc_idx{ (rho_idx * num_theta_bins) + theta_idx };
                const uint32_t votes{ accumulator[acc_idx] };
                if (votes < static_cast<uint32_t>(parameters.vote_min_threshold)) {
                    continue;
                }

                if (!isLocalMax(rho_idx, theta_idx, votes)) {
                    continue;
                }

                const float rho{ (static_cast<float>(rho_idx) * parameters.rho_resolution) - rho_max };
                const float theta{ static_cast<float>(theta_idx) * parameters.theta_resolution };
                detected_lines.emplace_back(HoughLine{ rho, theta, votes});
            }
        }
    }

    void HoughLineShapeDetectionComponent::processDetectedLines() {
        std::sort(detected_lines.begin(), detected_lines.end(),
            [](const HoughLine& a, const HoughLine& b) {
                return a.votes > b.votes;
            }
        );

        std::vector<HoughLine> valid_lines;
        for (HoughLine& line : detected_lines) {
            const std::vector<Pixel> line_pixels{ getLinePixels(line) };
            if (!lineSegmentExtraction(line_pixels, line)) {
                continue;
            }
            drawLine(line);
            valid_lines.push_back(line);
        }

        detected_lines = std::move(valid_lines);
    }

    void HoughLineShapeDetectionComponent::processContext(const Context& context) {
        ShapeDetectionComponent::processContext(context);
        rho_max        = std::sqrt(static_cast<float>((height * height) + (width * width)));
        num_rho_bins   = static_cast<uint32_t>(std::ceil(2 * rho_max / parameters.rho_resolution)) + 1U;

        detected_lines.clear();
        accumulator.assign(static_cast<size_t>(num_rho_bins) * num_theta_bins, 0U);

        cos_table.resize(num_theta_bins);
        sin_table.resize(num_theta_bins);
        for (uint32_t theta_idx{ 0U }; theta_idx < num_theta_bins; ++theta_idx) {
            const float theta{ static_cast<float>(theta_idx) * parameters.theta_resolution };
            cos_table[theta_idx] = std::cos(theta);
            sin_table[theta_idx] = std::sin(theta);
        }
    }

    void HoughLineShapeDetectionComponent::applyShapeDetection() {
        applyHoughTransform();
        nonMaximumSuppression();
        processDetectedLines();
    }

    bool HoughLineShapeDetectionComponent::isLocalMax(uint32_t rho_idx, uint32_t theta_idx, uint32_t votes) const {
        for (int drho{ -1 }; drho <= 1; ++drho) {
            for (int dtheta{ -1 }; dtheta <= 1; ++dtheta) {
                if (drho == 0 && dtheta == 0) {
                    continue;
                }
                const uint32_t neighbor_idx{
                    (static_cast<uint32_t>(static_cast<int>(rho_idx) + drho) * num_theta_bins) +
                    static_cast<uint32_t>(static_cast<int>(theta_idx) + dtheta)
                };
                if (accumulator[neighbor_idx] > votes) {
                    return false;
                }
            }
        }
        return true;
    }

    std::vector<HoughLineShapeDetectionComponent::Pixel> HoughLineShapeDetectionComponent::getLinePixels(const HoughLine& line) const {
        const float cos_theta{ std::cos(line.theta) };
        const float sin_theta{ std::sin(line.theta) };

        std::vector<Pixel> line_pixels;
        // horizontal-ish line
        if (std::abs(sin_theta) > std::abs(cos_theta)) {
            for (PixelIdx x{ 0U }; x < width; ++x) {
                const float y_float{ std::round((line.rho - static_cast<float>(x) * cos_theta) / sin_theta) };
                if (0.0f <= y_float && y_float < static_cast<float>(height)) {
                    line_pixels.emplace_back(x, static_cast<PixelIdx>(y_float));
                }
            }
        }
        // vertical-ish line
        else {
            for (PixelIdx y{ 0U }; y < height; ++y) {
                const float x_float{ std::round((line.rho - static_cast<float>(y) * sin_theta) / cos_theta) };
                if (0.0f <= x_float && x_float < static_cast<float>(width)) {
                    line_pixels.emplace_back(static_cast<PixelIdx>(x_float), y);
                }
            }
        }

        return line_pixels;
    }

    bool HoughLineShapeDetectionComponent::lineSegmentExtraction(const std::vector<Pixel>& line_pixels, HoughLine& line) const {
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
                if (gap_count > static_cast<uint32_t>(parameters.max_line_gap)) {
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

        if (best_run_length < static_cast<uint32_t>(parameters.min_line_length)) {
            return false;
        }

        line.x0 = line_pixels[static_cast<size_t>(best_run_start)].first;
        line.y0 = line_pixels[static_cast<size_t>(best_run_start)].second;
        line.x1 = line_pixels[static_cast<size_t>(best_run_end)].first;
        line.y1 = line_pixels[static_cast<size_t>(best_run_end)].second;

        return true;
    }

    void HoughLineShapeDetectionComponent::drawLine(const HoughLine& line) {
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
    }
} // namespace components::shape_detection
