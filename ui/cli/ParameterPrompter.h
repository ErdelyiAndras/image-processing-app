#ifndef PARAMETER_PROMPTER_H
#define PARAMETER_PROMPTER_H

#include "NodeTypes.h"
#include "ParameterValidator.h"

#include <iomanip>
#include <iostream>

class ParameterPrompter {
public:
    ParameterPrompter() = delete;

    static NodeParams prompt(const NodeParams& current);

    static void print(const NodeParams& params);

private:
    static void printImpl(const TVParams&     p);
    static void printImpl(const GaussParams&  p);
    static void printImpl(const SobelParams&  p);
    static void printImpl(const CannyParams&  p);
    static void printImpl(const HoughLParams& p);
    static void printImpl(const HoughCParams& p);

    static TVParams     createCandidate(const TVParams&     current);
    static GaussParams  createCandidate(const GaussParams&  current);
    static SobelParams  createCandidate(const SobelParams&  current);
    static CannyParams  createCandidate(const CannyParams&  current);
    static HoughLParams createCandidate(const HoughLParams& current);
    static HoughCParams createCandidate(const HoughCParams& current);

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
