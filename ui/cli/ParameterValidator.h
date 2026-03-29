#ifndef PARAMETER_VALIDATOR_H
#define PARAMETER_VALIDATOR_H

#include "NodeTypes.h"
#include "config.h"

#include <iostream>
#include <string>
#include <vector>

class ParameterValidator {
public:
    ParameterValidator() = delete;

    struct ValidationResult {
        bool                     ok{ true };
        std::vector<std::string> errors;
    };

    static ValidationResult validate(const TVParams& p) {
        ValidationResult r;
        if (p.strength <= 0.0f) {
            addError(r, "Strength must be > 0.");
        }
        if (p.step_size <= 0.0f) {
            addError(r, "Step size must be > 0.");
        }
        if (p.tolerance <= 0.0f) {
            addError(r, "Tolerance must be > 0.");
        }
        return r;
    }

    static ValidationResult validate(const GaussParams& p) {
        ValidationResult r;
        if (p.kernel_size < 1) {
            addError(r, "Kernel size must be >= 1.");
        }
        if (p.kernel_size % 2 == 0) {
            addError(r, "Kernel size must be odd.");
        }
        if (p.sigma <= 0.0f) {
            addError(r, "Sigma must be > 0.");
        }
        return r;
    }

    static ValidationResult validate(const SobelParams& p) {
        ValidationResult r;
        if (p.threshold < 0.0f || p.threshold > 1.0f) {
            addError(r, "Threshold must be in [0, 1].");
        }
        return r;
    }

    static ValidationResult validate(const CannyParams& p) {
        ValidationResult r;
        if (p.low_threshold < 0.0f || p.low_threshold > 1.0f) {
            addError(r, "Low threshold must be in [0, 1].");
        }
        if (p.high_threshold < 0.0f || p.high_threshold > 1.0f) {
            addError(r, "High threshold must be in [0, 1].");
        }
        if (p.low_threshold >= p.high_threshold) {
            addError(r, "Low threshold must be strictly less than high threshold.");
        }
        return r;
    }

    static ValidationResult validate(const HoughLParams& p) {
        ValidationResult r;
        if (p.rho_resolution <= 0.0f) {
            addError(r, "Rho resolution must be > 0.");
        }
        if (p.theta_resolution <= 0.0f) {
            addError(r, "Theta resolution must be > 0.");
        }
        if (p.theta_resolution > pi) {
            addError(r, "Theta resolution must be <= pi (180 deg).");
        }
        if (p.vote_min_threshold < 1U) {
            addError(r, "Vote threshold must be >= 1.");
        }
        if (p.min_line_length < 1U) {
            addError(r, "Min line length must be >= 1.");
        }
        return r;
    }

    static ValidationResult validate(const HoughCParams& p) {
        ValidationResult r;
        if (p.vote_min_threshold < 1U) {
            addError(r, "Vote threshold must be >= 1.");
        }
        if (p.min_radius < 1U) {
            addError(r, "Min radius must be >= 1.");
        }
        if (p.max_radius < p.min_radius) {
            addError(r, "Max radius must be >= min radius.");
        }
        if (p.min_dist <= 0.0f) {
            addError(r, "Min distance must be > 0.");
        }
        if (p.num_angle_steps < 4U) {
            addError(r, "Angle steps must be >= 4.");
        }
        return r;
    }

    static ValidationResult validate(const NodeParams& params) {
        return std::visit([](const auto& p) {
            using T = std::decay_t<decltype(p)>;
            if constexpr (std::is_same_v<T, std::monostate>)
                return ValidationResult{};
            else
                return ParameterValidator::validate(p);
        }, params);
    }

    static void printErrors(const ValidationResult& result) {
        std::cout << "\n  Validation failed:\n";
        for (const std::string& err : result.errors)
            std::cout << "    * " << err << "\n";
        std::cout << "  Please correct the highlighted fields.\n\n";
    }

private:
    static void addError(ValidationResult& r, std::string message) {
        r.ok = false;
        r.errors.push_back(std::move(message));
    }
};

#endif // PARAMETER_VALIDATOR_H
