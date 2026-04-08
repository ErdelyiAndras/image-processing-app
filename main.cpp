#include "PipelineUI.h"

#include <exception>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    try {
        const std::string input_path  = (argc > 1) ? argv[1] : "";
        const std::string output_path = (argc > 2) ? argv[2] : "";

        PipelineUI ui(input_path, output_path);
        ui.run();
    }
    catch (const std::exception& ex) {
        std::cerr << "Unexpected error: " << ex.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "Unexpected error.\n";
        return 1;
    }

    return 0;
}
