#include "ParameterPrompter.h"
#include "NodeTypes.h"
#include "Terminal.h"

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

NodeParams ParameterPrompter::prompt(const NodeParams& current) {
    return std::visit([](const auto& p) -> NodeParams {
        using T = std::decay_t<decltype(p)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            return std::monostate{};
        } else {
            return createCandidate(p);
        }
    }, current);
}

void ParameterPrompter::print(const NodeParams& params) {
    std::visit([](const auto& p) {
        using T = std::decay_t<decltype(p)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            std::cout << "  (no configurable parameters)\n";
        } else {
            printImpl(p);
        }
    }, params);
}

void ParameterPrompter::printErrors(const ParameterValidator::ValidationResult& result) {
    std::cout << "\n  Validation failed:\n";
    for (const auto& [field, errors] : result) {
        std::cout << "    " << field << ":\n";
        for (const std::string& message : errors) {
            std::cout << "      - " << message << "\n";
        }
    }
    std::cout << "  Please correct the highlighted fields.\n\n";
}

void ParameterPrompter::printImpl(const TVParams& p) {
    printField("Strength",  p.strength);
    printField("Step size", p.step_size);
    printField("Tolerance", p.tolerance);
}

void ParameterPrompter::printImpl(const GaussParams& p) {
    printField("Kernel size", p.kernel_size);
    printField("Sigma",       p.sigma);
}

void ParameterPrompter::printImpl(const SobelParams& p) {
    printField("Threshold", p.threshold);
}

void ParameterPrompter::printImpl(const CannyParams& p) {
    printField("Low threshold",  p.low_threshold);
    printField("High threshold", p.high_threshold);
}

void ParameterPrompter::printImpl(const HoughLParams& p) {
    printField("Rho resolution",   p.rho_resolution);
    printField("Theta resolution", p.theta_resolution);
    printField("Vote threshold",   p.vote_min_threshold);
    printField("Min line length",  p.min_line_length);
    printField("Max line gap",     p.max_line_gap);
}

void ParameterPrompter::printImpl(const HoughCParams& p) {
    printField("Vote threshold", p.vote_min_threshold);
    printField("Min radius",     p.min_radius);
    printField("Max radius",     p.max_radius);
    printField("Min distance",   p.min_dist);
    printField("Angle steps",    p.num_angle_steps);
}

TVParams ParameterPrompter::createCandidate(const TVParams& current) {
    return TVParams{
        Terminal::readFloat("Strength",  current.strength),
        Terminal::readFloat("Step size", current.step_size),
        Terminal::readFloat("Tolerance", current.tolerance)
    };
}

GaussParams ParameterPrompter::createCandidate(const GaussParams& current) {
    return GaussParams{
        Terminal::readInt  ("Kernel size (odd)", current.kernel_size),
        Terminal::readFloat("Sigma",             current.sigma)
    };
}

SobelParams ParameterPrompter::createCandidate(const SobelParams& current) {
    return SobelParams{
        Terminal::readFloat("Threshold", current.threshold)
    };
}

CannyParams ParameterPrompter::createCandidate(const CannyParams& current) {
    return CannyParams{
        Terminal::readFloat("Low threshold",  current.low_threshold),
        Terminal::readFloat("High threshold", current.high_threshold)
    };
}

HoughLParams ParameterPrompter::createCandidate(const HoughLParams& current) {
    return HoughLParams{
        Terminal::readFloat("Rho resolution",   current.rho_resolution),
        Terminal::readFloat("Theta resolution", current.theta_resolution),
        static_cast<uint32_t>(Terminal::readInt("Vote threshold",  static_cast<int>(current.vote_min_threshold))),
        static_cast<uint32_t>(Terminal::readInt("Min line length", static_cast<int>(current.min_line_length))),
        static_cast<uint32_t>(Terminal::readInt("Max line gap",    static_cast<int>(current.max_line_gap)))
    };
}

HoughCParams ParameterPrompter::createCandidate(const HoughCParams& current) {
    return HoughCParams{
        static_cast<uint32_t>(Terminal::readInt("Vote threshold", static_cast<int>(current.vote_min_threshold))),
        static_cast<uint32_t>(Terminal::readInt("Min radius",     static_cast<int>(current.min_radius))),
        static_cast<uint32_t>(Terminal::readInt("Max radius",     static_cast<int>(current.max_radius))),
        Terminal::readFloat("Min distance",   current.min_dist),
        static_cast<uint32_t>(Terminal::readInt("Angle steps",    static_cast<int>(current.num_angle_steps)))
    };
}
