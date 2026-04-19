#include "PipelineUI.h"

#include <exception>
#include <iostream>

int main() {
    try {
        PipelineUI ui;
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
