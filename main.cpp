#include "PipelineUI.h"

#include <iostream>
#include <string>
#include <iomanip>

int main(int argc, char** argv) {
    const std::string inputPath  = (argc > 1) ? argv[1] : "";
    const std::string outputPath = (argc > 2) ? argv[2] : "";

    PipelineUI ui(inputPath, outputPath);
    ui.run();

    return 0;
}
