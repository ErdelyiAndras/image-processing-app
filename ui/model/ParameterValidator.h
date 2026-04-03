#ifndef PARAMETER_VALIDATOR_H
#define PARAMETER_VALIDATOR_H

#include "NodeTypes.h"
#include "config.h"

#include <string>
#include <vector>
#include <map>

class ParameterValidator {
public:
    ParameterValidator() = delete;

    class ValidationResult {
    public:
        using ErrorMap = std::map<std::string, std::vector<std::string>>;

        bool ok() const {
            return errors.empty();
        }

        ErrorMap::const_iterator begin() const {
            return errors.cbegin();
        }

        ErrorMap::const_iterator end() const {
            return errors.cend();
        }

        void addError(std::string field, std::string message) {
            errors[field].push_back(std::move(message));
        }
    private:
        ErrorMap errors;
    };

    static ValidationResult validate(const TVParams& p) {
        ValidationResult r;
        if (p.strength <= 0.0f) {
            r.addError("Strength", "must be > 0");
        }
        if (p.step_size <= 0.0f) {
            r.addError("Step size", "must be > 0");
        }
        if (p.tolerance <= 0.0f) {
            r.addError("Tolerance", "must be > 0");
        }
        return r;
    }

    static ValidationResult validate(const GaussParams& p) {
        ValidationResult r;
        if (p.kernel_size < 1) {
            r.addError("Kernel size", "must be >= 1");
        }
        if (p.kernel_size % 2 == 0) {
            r.addError("Kernel size", "must be odd");
        }
        if (p.sigma <= 0.0f) {
            r.addError("Sigma", "must be > 0");
        }
        return r;
    }

    static ValidationResult validate(const SobelParams& p) {
        ValidationResult r;
        if (p.threshold < 0.0f || p.threshold > 1.0f) {
            r.addError("Threshold", "must be in [0, 1]");
        }
        return r;
    }

    static ValidationResult validate(const CannyParams& p) {
        ValidationResult r;
        if (p.low_threshold < 0.0f || p.low_threshold > 1.0f) {
            r.addError("Low threshold", "must be in [0, 1]");
        }
        if (p.high_threshold < 0.0f || p.high_threshold > 1.0f) {
            r.addError("High threshold", "must be in [0, 1]");
        }
        if (p.low_threshold >= p.high_threshold) {
            r.addError("Low threshold", "must be strictly less than high threshold");
        }
        return r;
    }

    static ValidationResult validate(const HoughLParams& p) {
        ValidationResult r;
        if (p.rho_resolution <= 0.0f) {
            r.addError("Rho resolution", "must be > 0");
        }
        if (p.theta_resolution <= 0.0f) {
            r.addError("Theta resolution", "must be > 0");
        }
        if (p.theta_resolution > pi) {
            r.addError("Theta resolution", "must be <= pi (180 deg)");
        }
        if (p.vote_min_threshold < 1U) {
            r.addError("Vote threshold", "must be >= 1");
        }
        if (p.min_line_length < 1U) {
            r.addError("Min line length", "must be >= 1");
        }
        return r;
    }

    static ValidationResult validate(const HoughCParams& p) {
        ValidationResult r;
        if (p.vote_min_threshold < 1U) {
            r.addError("Vote threshold", "must be >= 1");
        }
        if (p.min_radius < 1U) {
            r.addError("Min radius", "must be >= 1");
        }
        if (p.max_radius < p.min_radius) {
            r.addError("Max radius", "must be >= min radius");
        }
        if (p.min_dist <= 0.0f) {
            r.addError("Min distance", "must be > 0");
        }
        if (p.num_angle_steps < 4U) {
            r.addError("Angle steps", "must be >= 4");
        }
        return r;
    }
};

#endif // PARAMETER_VALIDATOR_H
