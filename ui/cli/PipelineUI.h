#ifndef PIPELINE_UI_H
#define PIPELINE_UI_H

#include "PipelineController.h"
#include "Terminal.h"

#include <iostream>
#include <string>

class PipelineUI {
public:
    void run() {
        int option;
        do {
            displayMainMenu();
            option = Terminal::readChoice(0, 9);
            std::cout << "\nChosen: " << option << "\n";

            switch (option) {
                case 1:  controller.setInput();        break;
                case 2:  controller.setOutput();       break;
                case 3:  controller.addNode();         break;
                case 4:  controller.removeNode();      break;
                case 5:  controller.connectNodes();    break;
                case 6:  controller.disconnectNodes(); break;
                case 7:  controller.configureNode();   break;
                case 8:  controller.listPipeline();    break;
                case 9:  controller.run();             break;
                case 0:  std::cout << "\n" << Terminal::indent << "Goodbye.\n";  break;
                default: std::cout << Terminal::indent << "Invalid choice.\n"; break;
            }

            std::cout << "\n";
        } while (option != 0);
    }

private:
    PipelineController controller;

    void displayMainMenu() const {
        Terminal::header("Image Processing Pipeline Builder");
        std::cout << Terminal::indent << "Input  : " << (controller.getInputPath().empty()  ? "(not set)" : controller.getInputPath())  << "\n"
                  << Terminal::indent << "Output : " << (controller.getOutputPath().empty() ? "(not set)" : controller.getOutputPath()) << "\n"
                  << Terminal::indent << "Nodes  : " << controller.nodeCount() << "\n\n"
                  << Terminal::indent << "1. Set input image\n"
                  << Terminal::indent << "2. Set output path\n"
                  << Terminal::indent << "3. Add node\n"
                  << Terminal::indent << "4. Remove node\n"
                  << Terminal::indent << "5. Connect nodes\n"
                  << Terminal::indent << "6. Disconnect nodes\n"
                  << Terminal::indent << "7. Configure node\n"
                  << Terminal::indent << "8. List pipeline\n"
                  << Terminal::indent << "9. Run pipeline\n"
                  << Terminal::indent << "0. Exit\n\n";
    }
};

#endif // PIPELINE_UI_H
