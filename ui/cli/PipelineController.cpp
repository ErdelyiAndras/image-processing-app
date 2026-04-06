#include "PipelineController.h"
#include "ComponentRegistry.h"
#include "Context.h"
#include "Image.h"
#include "NodeTypes.h"
#include "ParameterPrompter.h"
#include "ParameterValidator.h"
#include "PipelineModel.h"
#include "Terminal.h"

#include <algorithm>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

PipelineController::PipelineController(std::string inputPath, std::string outputPath)
    : model(std::move(inputPath), std::move(outputPath)) {}

void PipelineController::setInput() {
    Terminal::header("Set Input Image");
    const std::string path{ Terminal::readString("Image path", model.getInputPath()) };
    if (!path.empty()) {
        model.setInputPath(path);
        std::cout << "\n  Input set to: " << model.getInputPath() << "\n";
    } else {
        std::cout << "  No change.\n";
    }
}

void PipelineController::setOutput() {
    Terminal::header("Set Output Path");
    std::cout << "  Include extension, e.g. output/result.png\n\n";
    const std::string path{ Terminal::readString("Output path", model.getOutputPath()) };
    if (!path.empty()) {
        model.setOutputPath(path);
        std::cout << "\n  Output set to: " << model.getOutputPath() << "\n";
    } else {
        std::cout << "  No change.\n";
    }
}

void PipelineController::addNode() {
    Terminal::header("Add Node");

    for (uint8_t i{ 0U }; i < static_cast<uint8_t>(Category::CategoryCount); ++i) {
        std::cout << "  " << (i + 1) << ". "
                  << ComponentRegistry::categoryName(static_cast<Category>(i))
                  << "\n";
    }
    std::cout << "  0. Cancel\n\n";

    const int cat{ Terminal::readChoice(0, static_cast<int>(Category::CategoryCount)) };
    if (cat == 0) {
        return;
    }

    const Category chosenCategory{ static_cast<Category>(cat - 1) };
    const std::vector<const ComponentDescriptor*> components{ ComponentRegistry::byCategory(chosenCategory) };

    std::cout << "\n  " << ComponentRegistry::categoryName(chosenCategory) << ":\n";
    for (size_t i{ 0 }; i < components.size(); ++i) {
        std::cout << "    " << (i + 1) << ". " << components[i]->displayName() << "\n";
    }
    std::cout << "    0. Cancel\n\n";

    const int comp{ Terminal::readChoice(0, static_cast<int>(components.size())) };
    if (comp == 0) {
        return;
    }

    const ComponentDescriptor& desc{ *components[static_cast<size_t>(comp - 1)] };

    std::cout << "\n";
    const std::string name{ Terminal::readString("Node name", desc.displayName()) };

    if (desc.hasParams()) {
        std::cout << "\n  Parameters (press Enter to accept default):\n";
    }
    NodeParams current{ desc.hasParams() ? desc.defaultParams() : std::monostate{} };
    while (true) {
        NodeParams candidate{ ParameterPrompter::prompt(current) };

        try {
            PipelineModel::AddNodeResult result{ model.addNode(desc, candidate, name) };
            if (result.validation.ok()) {
                std::cout << "\n  Node " << *result.id << " added: ["
                          << desc.displayName() << "] \"" << name << "\"\n";
                break;
            }
            ParameterPrompter::printErrors(result.validation);
            current = std::move(candidate);
        } catch (const std::exception& e) {
            std::cout << "\n  Error: " << e.what() << "\n";
            break;
        }
    }
}

void PipelineController::removeNode() {
    Terminal::header("Remove Node");
    printNodeTable();
    if (model.nodeCount() == 0) {
        return;
    }

    std::cout << "\n";
    NodeId id;
    if (!askNodeId("Node ID to remove", id)) {
        return;
    }

    try {
        const std::string name{ model.node(id).displayName };
        model.removeNode(id);
        std::cout << "  Node " << id << " (\"" << name << "\") removed.\n";
    } catch (const std::exception& e) {
        std::cout << "  Error: " << e.what() << "\n";
    }
}

void PipelineController::connectNodes() {
    Terminal::header("Connect Nodes");
    printNodeTable();
    if (model.nodeCount() < 2) {
        std::cout << "  Need at least two nodes to connect.\n";
        return;
    }

    std::cout << "\n";
    NodeId from, to;
    if (!askNodeId("From node ID", from)) {
        return;
    }
    if (!askNodeId("To node ID", to)) {
        return;
    }

    try {
        model.connect(from, to);
        std::cout << "  Connected: "
                  << from << " [" << model.node(from).displayName << "]"
                  << "  -->  "
                  << to   << " [" << model.node(to).displayName   << "]\n";
    } catch (const std::exception& e) {
        std::cout << "  Error: " << e.what() << "\n";
    }
}

void PipelineController::disconnectNodes() {
    Terminal::header("Disconnect Nodes");
    printConnectionList();
    if (model.getConnections().empty()) {
        return;
    }

    std::cout << "\n";
    NodeId from, to;
    if (!askNodeId("From node ID", from)) {
        return;
    }
    if (!askNodeId("To node ID", to)) {
        return;
    }

    try {
        model.disconnect(from, to);
        std::cout << "  Disconnected: " << from << " --> " << to << "\n";
    } catch (const std::exception& e) {
        std::cout << "  Error: " << e.what() << "\n";
    }
}

void PipelineController::configureNode() {
    Terminal::header("Configure Node");
    printNodeTable();
    if (model.nodeCount() == 0) {
        return;
    }

    std::cout << "\n";
    NodeId id;
    if (!askNodeId("Node ID to configure", id)) {
        return;
    }

    const NodeInfo& info{ model.node(id) };
    const ComponentDescriptor& desc{ ComponentRegistry::get(info.type) };

    if (!desc.hasParams()) {
        std::cout << "  [" << desc.displayName()
                  << "] has no configurable parameters.\n";
        return;
    }

    std::cout << "\n  Node " << id << ": [" << desc.displayName()
              << "] \"" << info.displayName << "\"\n"
              << "  Current parameters:\n";
    ParameterPrompter::print(info.params);
    std::cout << "\n  New parameters (press Enter to keep current value):\n";

    NodeParams current{ info.params };
    while (true) {
        NodeParams candidate{ ParameterPrompter::prompt(current) };

        try {
            ParameterValidator::ValidationResult result{ model.configureNode(id, candidate) };
            if (result.ok()) {
                std::cout << "\n  Parameters updated.\n";
                break;
            }
            ParameterPrompter::printErrors(result);
            current = std::move(candidate);
        } catch (const std::exception& e) {
            std::cout << "  Error: " << e.what() << "\n";
            break;
        }
    }
}

void PipelineController::listPipeline() const {
    Terminal::header("Pipeline Overview");
    const std::string& in{ model.getInputPath() };
    const std::string& out{ model.getOutputPath() };
    std::cout << "  Input  : " << (in.empty()  ? "(not set)" : in)  << "\n";
    std::cout << "  Output : " << (out.empty() ? "(not set)" : out) << "\n\n";
    std::cout << "  Nodes:\n";
    printNodeTable();
    std::cout << "\n  Connections:\n";
    printConnectionList();
}

void PipelineController::run() {
    Terminal::header("Run Pipeline");

    try {
        std::cout << "  Loading: " << model.getInputPath() << "\n";
        std::cout << "  Executing pipeline...\n";

        auto result{ model.execute() };

        std::cout << "  Done in " << std::fixed << std::setprecision(3)
                  << result.elapsedSeconds << " s\n\n";
        std::cout << "  Saving " << result.outputs.size() << " output(s):\n";

        const std::string& outputPath{ model.getOutputPath() };
        const size_t dotPos{ outputPath.find_last_of('.') };
        const std::string base{
            (dotPos == std::string::npos) ? outputPath : outputPath.substr(0, dotPos)
        };
        const std::string ext{
            (dotPos == std::string::npos) ? std::string{ ".png" } : outputPath.substr(dotPos)
        };

        for (auto& [id, ctx] : result.outputs) {
            const std::string label{ ctx.getAppliedComponents() };
            const std::string current_base{ base + "_" + std::to_string(id) };

            const auto makeName{ [&](const char* suffix) {
                return current_base + "_" + suffix + "_" + label;
            } };

            ctx.save(
                makeName("output"), ext
            );
            ctx.getProcessedImage().save(
                makeName("processed"), ext
            );
            ctx.getEdgeMap().save(
                makeName("edge"), ext
            );
            ctx.getShapeMap().save(
                makeName("shape"), ext
            );
            std::cout << "    [sink " << id << "] "
                      << makeName("output") << ext << "\n";
        }

    } catch (const std::exception& e) {
        std::cout << "  Error: " << e.what() << "\n";
    }
}

bool PipelineController::askNodeId(const std::string& prompt, NodeId& out) const {
    static constexpr int cancelValue{ -1 };
    while (true) {
        const int v{ Terminal::readNodeId(prompt, cancelValue) };
        if (v == cancelValue) {
            return false;
        }
        if (v >= 0) {
            const NodeId id{ static_cast<NodeId>(v) };
            if (model.hasNode(id)) {
                out = id;
                return true;
            }
            std::cout << "  Node " << v << " does not exist. Try again.\n";
        } else {
            std::cout << "  Invalid ID.\n";
        }
    }
}

void PipelineController::printNodeTable() const {
    if (model.nodeCount() == 0) {
        std::cout << "  (no nodes)\n";
        return;
    }

    std::cout << "  " << std::left
              << std::setw(5)  << "ID"
              << std::setw(32) << "Type"
              << "Name\n";
    Terminal::separator();

    std::vector<NodeId> ids;
    ids.reserve(model.nodeCount());
    for (const auto& [key, _] : model.nodes()) {
        ids.push_back(key);
    }
    std::sort(ids.begin(), ids.end());

    for (const NodeId id : ids) {
        const NodeInfo& info{ model.node(id) };
        const ComponentDescriptor& desc{ ComponentRegistry::get(info.type) };
        std::cout << "  " << std::left
                  << std::setw(5)  << id
                  << std::setw(32) << desc.displayName()
                  << info.displayName << "\n";
    }
}

void PipelineController::printConnectionList() const {
    const auto& connections{ model.getConnections() };
    if (connections.empty()) {
        std::cout << "  (no connections)\n";
        return;
    }
    for (const auto& [from, to] : connections) {
        std::cout << "  " << from << " [" << model.node(from).displayName << "]"
                  << "  -->  "
                  << to   << " [" << model.node(to).displayName   << "]\n";
    }
}
