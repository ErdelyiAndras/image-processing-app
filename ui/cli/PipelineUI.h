#ifndef PIPELINE_UI_H
#define PIPELINE_UI_H

#include "PipelineController.h"
#include "Terminal.h"

#include <iostream>
#include <string>

class PipelineUI {
public:
    explicit PipelineUI(std::string inputPath = "", std::string outputPath = "")
        : m_controller(std::move(inputPath), std::move(outputPath)) {}

    void run() {
        int option;
        do {
            displayMainMenu();
            option = Terminal::readChoice(0, 9);
            std::cout << "\nChosen: " << option << "\n";

            switch (option) {
                case 1:  m_controller.setInput();        break;
                case 2:  m_controller.setOutput();       break;
                case 3:  m_controller.addNode();         break;
                case 4:  m_controller.removeNode();      break;
                case 5:  m_controller.connectNodes();    break;
                case 6:  m_controller.disconnectNodes(); break;
                case 7:  m_controller.configureNode();   break;
                case 8:  m_controller.listPipeline();    break;
                case 9:  m_controller.run();             break;
                case 0:  std::cout << "\n  Goodbye.\n";  break;
                default: std::cout << "  Invalid choice.\n"; break;
            }

            std::cout << "\n";
        } while (option != 0);
    }

private:
    PipelineController m_controller;

    void displayMainMenu() const {
        Terminal::header("Image Processing Pipeline Builder");
        std::cout << "  Input  : " << (m_controller.getInputPath().empty()  ? "(not set)" : m_controller.getInputPath())  << "\n"
                  << "  Output : " << (m_controller.getOutputPath().empty() ? "(not set)" : m_controller.getOutputPath()) << "\n"
                  << "  Nodes  : " << m_controller.nodeCount() << "\n\n"
                  << "  1. Set input image\n"
                  << "  2. Set output path\n"
                  << "  3. Add node\n"
                  << "  4. Remove node\n"
                  << "  5. Connect nodes\n"
                  << "  6. Disconnect nodes\n"
                  << "  7. Configure node\n"
                  << "  8. List pipeline\n"
                  << "  9. Run pipeline\n"
                  << "  0. Exit\n\n";
    }
};

#endif // PIPELINE_UI_H
