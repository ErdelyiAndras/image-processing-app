#ifndef TERMINAL_H
#define TERMINAL_H

#include <iomanip>
#include <iostream>
#include <string>

class Terminal {
public:
    Terminal() = delete;

    static void separator(char c = '-', size_t w = 56) {
        std::cout << std::string(w, c) << "\n";
    }

    static void header(const std::string& title) {
        std::cout << "\n";
        separator('=');
        std::cout << "  " << title << "\n";
        separator('=');
    }

    static int readChoice(int lo, int hi) {
        std::string line;
        while (true) {
            std::cout << "Choice: ";
            std::getline(std::cin, line);
            if (!line.empty()) {
                try {
                    size_t pos;
                    const int v{ std::stoi(line, &pos) };
                    if (pos == line.size() && v >= lo && v <= hi) {
                        return v;
                    }
                } catch (...) {}
            }
            std::cout << "  Please enter a number between " << lo << " and " << hi << ".\n";
        }
    }

    static int readNodeId(const std::string& prompt, int cancelValue = -1) {
        std::string line;
        while (true) {
            std::cout << "  " << prompt << " (" << cancelValue << " to cancel): ";
            std::getline(std::cin, line);
            if (!line.empty()) {
                try {
                    size_t pos;
                    const int v{ std::stoi(line, &pos) };
                    if (pos == line.size()) {
                        return v;
                    }
                } catch (...) {}
            }
            std::cout << "  Invalid. Enter an integer.\n";
        }
    }

    static float readFloat(const std::string& label, float def) {
        std::string line;
        while (true) {
            std::cout << "  " << std::left << std::setw(24) << label
                      << "[" << def << "]: ";
            std::getline(std::cin, line);
            if (line.empty()) {
                return def;
            }
            try {
                size_t pos;
                const float v{ std::stof(line, &pos) };
                if (pos == line.size()) {
                    return v;
                }
            } catch (...) {}
            std::cout << "  Invalid. Enter a decimal number.\n";
        }
    }

    static int readInt(const std::string& label, int def) {
        std::string line;
        while (true) {
            std::cout << "  " << std::left << std::setw(24) << label
                      << "[" << def << "]: ";
            std::getline(std::cin, line);
            if (line.empty()) {
                return def;
            }
            try {
                size_t pos;
                const int v{ std::stoi(line, &pos) };
                if (pos == line.size()) {
                    return v;
                }
            } catch (...) {}
            std::cout << "  Invalid. Enter an integer.\n";
        }
    }

    static std::string readString(const std::string& label, const std::string& def) {
        std::string line;
        std::cout << "  " << std::left << std::setw(24) << label;
        if (!def.empty()) {
            std::cout << "[" << def << "]";
        }
        std::cout << ": ";
        std::getline(std::cin, line);
        return line.empty() ? def : line;
    }
};

#endif // TERMINAL_H
