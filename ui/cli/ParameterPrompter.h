#ifndef PARAMETER_PROMPTER_H
#define PARAMETER_PROMPTER_H

#include "NodeTypes.h"
#include "Terminal.h"
#include "ParameterValidator.h"

#include <iomanip>
#include <iostream>
#include <type_traits>

class ParameterPrompter {
public:
    ParameterPrompter() = delete;

    template <typename ParamsT>
    static NodeParams promptTyped(const NodeParams& current) {
        return promptLoop(std::get<ParamsT>(current));
    }

    template <typename ParamsT>
    static void printTyped(const NodeParams& params) {
        printImpl(std::get<ParamsT>(params));
    }

private:
    static void printImpl(const TVParams& p) {
        printField("Strength",  p.strength);
        printField("Step size", p.step_size);
        printField("Tolerance", p.tolerance);
    }

    static void printImpl(const GaussParams& p) {
        printField("Kernel size", p.kernel_size);
        printField("Sigma",       p.sigma);
    }

    static void printImpl(const SobelParams& p) {
        printField("Threshold", p.threshold);
    }

    static void printImpl(const CannyParams& p) {
        printField("Low threshold",  p.low_threshold);
        printField("High threshold", p.high_threshold);
    }

    static void printImpl(const HoughLParams& p) {
        printField("Rho resolution",   p.rho_resolution);
        printField("Theta resolution", p.theta_resolution);
        printField("Vote threshold",   p.vote_min_threshold);
        printField("Min line length",  p.min_line_length);
        printField("Max line gap",     p.max_line_gap);
    }

    static void printImpl(const HoughCParams& p) {
        printField("Vote threshold", p.vote_min_threshold);
        printField("Min radius",     p.min_radius);
        printField("Max radius",     p.max_radius);
        printField("Min distance",   p.min_dist);
        printField("Angle steps",    p.num_angle_steps);
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
    static T promptLoop(const T& initial) {
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

    template <typename T>
    static void printField(const char* label, T value) {
        constexpr static int label_width{ 17 };
        std::cout << "  " << std::setw(label_width) << std::left
                  << label << ": " << value << "\n";
    }
};

#endif // PARAMETER_PROMPTER_H
