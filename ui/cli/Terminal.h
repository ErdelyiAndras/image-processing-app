#ifndef TERMINAL_H
#define TERMINAL_H

#include <cstddef>
#include <string>

class Terminal {
public:
    static const     std::string indent;
    static constexpr int         id_col_width     {  5 };
    static constexpr int         type_col_width   { 32 };
    static constexpr int         label_field_width{ 24 };

    Terminal() = delete;

    static void separator(char c = '-', size_t w = 56);
    static void header(const std::string& title);

    static int         readChoice(int lo, int hi);
    static int         readNodeId(const std::string& prompt, int cancelValue = -1);
    static float       readFloat (const std::string& label, float def);
    static int         readInt   (const std::string& label, int def);
    static std::string readString(const std::string& label, const std::string& def);
};

#endif // TERMINAL_H
