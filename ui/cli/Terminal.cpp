#include "Terminal.h"

#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>

const std::string Terminal::indent{ "  " };

void Terminal::separator(char c, size_t w) {
    std::cout << std::string(w, c) << "\n";
}

void Terminal::header(const std::string& title) {
    std::cout << "\n";
    separator('=');
    std::cout << indent << title << "\n";
    separator('=');
}

int Terminal::readChoice(int lo, int hi) {
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
        std::cout << indent << "Please enter a number between " << lo << " and " << hi << ".\n";
    }
}

int Terminal::readNodeId(const std::string& prompt, int cancelValue) {
    std::string line;
    while (true) {
        std::cout << indent << prompt << " (" << cancelValue << " to cancel): ";
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
        std::cout << indent << "Invalid. Enter an integer.\n";
    }
}

float Terminal::readFloat(const std::string& label, float def) {
    std::string line;
    while (true) {
        std::cout << indent << std::left << std::setw(label_field_width) << label
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
        std::cout << indent << "Invalid. Enter a decimal number.\n";
    }
}

int Terminal::readInt(const std::string& label, int def) {
    std::string line;
    while (true) {
        std::cout << indent << std::left << std::setw(label_field_width) << label
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
        std::cout << indent << "Invalid. Enter an integer.\n";
    }
}

std::string Terminal::readString(const std::string& label, const std::string& def) {
    std::string line;
    std::cout << indent << std::left << std::setw(label_field_width) << label;
    if (!def.empty()) {
        std::cout << "[" << def << "]";
    }
    std::cout << ": ";
    std::getline(std::cin, line);
    return line.empty() ? def : line;
}
