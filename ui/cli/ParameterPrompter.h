#ifndef PARAMETER_PROMPTER_H
#define PARAMETER_PROMPTER_H

#include "NodeTypes.h"
#include "Terminal.h"
#include "ParameterValidator.h"
#include "config.h"
#include "denoising-config.h"
#include "edge-detection-config.h"
#include "shape-detection-config.h"

#include <iostream>
#include <type_traits>

class ParameterPrompter {
public:
    ParameterPrompter() = delete;

    static NodeParams promptFresh(ComponentType type) {
        switch (type) {
            case ComponentType::TVDenoisingCPU:
            case ComponentType::TVDenoisingGPU:
                return prompt(
                    TVParams{
                        components::denoising::default_strength,
                        components::denoising::default_step_size,
                        components::denoising::default_tolerance
                    }
                );

            case ComponentType::GaussianBlurCPU:
            case ComponentType::GaussianBlurGPU:
                return prompt(
                    GaussParams{
                        components::denoising::default_kernel_size,
                        components::denoising::default_sigma
                    }
                );

            case ComponentType::SobelCPU:
            case ComponentType::SobelGPU:
                return prompt(
                    SobelParams{
                        components::edge_detection::default_threshold
                    }
                );

            case ComponentType::CannyCPU:
            case ComponentType::CannyGPU:
                return prompt(
                    CannyParams{
                        components::edge_detection::default_low_threshold,
                        components::edge_detection::default_high_threshold
                    }
                );

            case ComponentType::HoughLineCPU:
            case ComponentType::HoughLineGPU:
                return prompt(
                    HoughLParams{
                        components::shape_detection::default_rho_resolution,
                        components::shape_detection::default_theta_resolution,
                        components::shape_detection::default_threshold,
                        components::shape_detection::default_min_line_length,
                        components::shape_detection::default_max_line_gap
                    }
                );

            case ComponentType::HoughCircleCPU:
            case ComponentType::HoughCircleGPU:
                return prompt(
                    HoughCParams{
                        components::shape_detection::default_circle_vote_min_threshold,
                        components::shape_detection::default_min_radius,
                        components::shape_detection::default_max_radius,
                        components::shape_detection::default_min_dist,
                        components::shape_detection::default_num_angle_steps
                    }
                );

            default:
                return std::monostate{};
        }
    }

    static NodeParams promptUpdate(ComponentType type, const NodeParams& current) {
        switch (type) {
            case ComponentType::TVDenoisingCPU:
            case ComponentType::TVDenoisingGPU:
                return prompt(std::get<TVParams>(current));

            case ComponentType::GaussianBlurCPU:
            case ComponentType::GaussianBlurGPU:
                return prompt(std::get<GaussParams>(current));

            case ComponentType::SobelCPU:
            case ComponentType::SobelGPU:
                return prompt(std::get<SobelParams>(current));

            case ComponentType::CannyCPU:
            case ComponentType::CannyGPU:
                return prompt(std::get<CannyParams>(current));

            case ComponentType::HoughLineCPU:
            case ComponentType::HoughLineGPU:
                return prompt(std::get<HoughLParams>(current));

            case ComponentType::HoughCircleCPU:
            case ComponentType::HoughCircleGPU:
                return prompt(std::get<HoughCParams>(current));

            default:
                return std::monostate{};
        }
    }

    static void printParams(const NodeInfo& info) {
        std::visit([](const auto& p) {
            using T = std::decay_t<decltype(p)>;
            if constexpr (std::is_same_v<T, TVParams>) {
                print("Strength",         p.strength);
                print("Step size",        p.step_size);
                print("Tolerance",        p.tolerance);
            } else if constexpr (std::is_same_v<T, GaussParams>) {
                print("Kernel size",      p.kernel_size);
                print("Sigma",            p.sigma);
            } else if constexpr (std::is_same_v<T, SobelParams>) {
                print("Threshold",        p.threshold);
            } else if constexpr (std::is_same_v<T, CannyParams>) {
                print("Low threshold",    p.low_threshold);
                print("High threshold",   p.high_threshold);
            } else if constexpr (std::is_same_v<T, HoughLParams>) {
                print("Rho resolution",   p.rho_resolution);
                print("Theta resolution", p.theta_resolution);
                print("Vote threshold",   p.vote_min_threshold);
                print("Min line length",  p.min_line_length);
                print("Max line gap",     p.max_line_gap);
            } else if constexpr (std::is_same_v<T, HoughCParams>) {
                print("Vote threshold",   p.vote_min_threshold);
                print("Min radius",       p.min_radius);
                print("Max radius",       p.max_radius);
                print("Min distance",     p.min_dist);
                print("Angle steps",      p.num_angle_steps);
            } else {
                std::cout << "  (no configurable parameters)\n";
            }
        }, info.params);
    }

private:
    template <typename T>
    static T prompt(const T& initial) {
        T current{ initial };
        while (true) {
            const T candidate{ createCandidate(current) };
            const ParameterValidator::ValidationResult result{
                ParameterValidator::validate(candidate)
            };
            if (result.ok) {
                return candidate;
            }
            ParameterValidator::printErrors(result);
            current = candidate;
        }
    }

    static TVParams createCandidate(const TVParams& current) {
        return TVParams{
            Terminal::readFloat("Strength",  current.strength),
            Terminal::readFloat("Step size", current.step_size),
            Terminal::readFloat("Tolerance", current.tolerance)
        };
    }

    static GaussParams createCandidate(const GaussParams& current) {
        return GaussParams{
            Terminal::readInt  ("Kernel size (odd)", current.kernel_size),
            Terminal::readFloat("Sigma",             current.sigma)
        };
    }

    static SobelParams createCandidate(const SobelParams& current) {
        return SobelParams{
            Terminal::readFloat("Threshold", current.threshold)
        };
    }

    static CannyParams createCandidate(const CannyParams& current) {
        return CannyParams{
            Terminal::readFloat("Low threshold",  current.low_threshold),
            Terminal::readFloat("High threshold", current.high_threshold)
        };
    }

    static HoughLParams createCandidate(const HoughLParams& current) {
        return HoughLParams{
            Terminal::readFloat("Rho resolution",   current.rho_resolution),
            Terminal::readFloat("Theta resolution", current.theta_resolution),
            static_cast<uint32_t>(Terminal::readInt("Vote threshold",  static_cast<int>(current.vote_min_threshold))),
            static_cast<uint32_t>(Terminal::readInt("Min line length", static_cast<int>(current.min_line_length))),
            static_cast<uint32_t>(Terminal::readInt("Max line gap",    static_cast<int>(current.max_line_gap)))
        };
    }

    static HoughCParams createCandidate(const HoughCParams& current) {
        return HoughCParams{
            static_cast<uint32_t>(Terminal::readInt("Vote threshold", static_cast<int>(current.vote_min_threshold))),
            static_cast<uint32_t>(Terminal::readInt("Min radius",     static_cast<int>(current.min_radius))),
            static_cast<uint32_t>(Terminal::readInt("Max radius",     static_cast<int>(current.max_radius))),
            Terminal::readFloat("Min distance",   current.min_dist),
            static_cast<uint32_t>(Terminal::readInt("Angle steps",    static_cast<int>(current.num_angle_steps)))
        };
    }

    template <typename T>
    static void print(const char* label, T value) {
        constexpr int label_width{ 17 };
        std::cout << "  " << std::setw(label_width) << std::left
                  << label << ": " << value << "\n";
    }
};

#endif // PARAMETER_PROMPTER_H
