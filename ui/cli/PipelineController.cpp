#include "PipelineController.h"

#include "ComponentRegistry.h"
#include "Image.h"
#include "NodeTypes.h"
#include "ParameterPrompter.h"
#include "ParameterValidator.h"
#include "Pipeline.h"
#include "PipelineModel.h"
#include "Terminal.h"
#include "types.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

PipelineController::PipelineController(std::string input_path, std::string output_path)
    : model(std::move(input_path), std::move(output_path)) {}

void PipelineController::setInput() {
    Terminal::header("Set Input Image");
    const std::string path{ Terminal::readString("Image path", model.getInputPath()) };
    if (!path.empty()) {
        model.setInputPath(path);
        std::cout << "\n" << Terminal::indent << "Input set to: " << model.getInputPath() << "\n";
    } else {
        std::cout << Terminal::indent << "No change.\n";
    }
}

void PipelineController::setOutput() {
    Terminal::header("Set Output Path");
    std::cout << Terminal::indent << "Include extension, e.g. output/result.png\n\n";
    const std::string path{ Terminal::readString("Output path", model.getOutputPath()) };
    if (!path.empty()) {
        model.setOutputPath(path);
        std::cout << "\n" << Terminal::indent << "Output set to: " << model.getOutputPath() << "\n";
    } else {
        std::cout << Terminal::indent << "No change.\n";
    }
}

void PipelineController::addNode() {
    Terminal::header("Add Node");

    for (uint8_t i{ 0U }; i < static_cast<uint8_t>(Category::CategoryCount); ++i) {
        std::cout << Terminal::indent << (i + 1) << ". "
                  << ComponentRegistry::categoryName(static_cast<Category>(i))
                  << "\n";
    }
    std::cout << Terminal::indent << "0. Cancel\n\n";

    const int cat{ Terminal::readChoice(0, static_cast<int>(Category::CategoryCount)) };
    if (cat == 0) {
        return;
    }

    const Category chosen_category{ static_cast<Category>(cat - 1) };
    const std::vector<const ComponentDescriptor*> components{ ComponentRegistry::byCategory(chosen_category) };

    std::cout << "\n" << Terminal::indent << ComponentRegistry::categoryName(chosen_category) << ":\n";
    for (size_t i{ 0 }; i < components.size(); ++i) {
        std::cout << Terminal::indent << Terminal::indent << (i + 1) << ". " << components[i]->displayName() << "\n";
    }
    std::cout << Terminal::indent << Terminal::indent << "0. Cancel\n\n";

    const int comp{ Terminal::readChoice(0, static_cast<int>(components.size())) };
    if (comp == 0) {
        return;
    }

    const ComponentDescriptor& desc{ *components[static_cast<size_t>(comp - 1)] };

    std::cout << "\n";
    const std::string name{ Terminal::readString("Node name", desc.displayName()) };

    if (desc.hasParams()) {
        std::cout << "\n" << Terminal::indent << "Parameters (press Enter to accept default):\n";
    }
    NodeParams current{ desc.hasParams() ? desc.defaultParams() : std::monostate{} };
    while (true) {
        NodeParams candidate{ ParameterPrompter::prompt(current) };

        try {
            PipelineModel::AddNodeResult result{ model.addNode(desc, candidate, name) };
            if (result.validation.ok()) {
                std::cout << "\n" << Terminal::indent << "Node " << *result.id << " added: ["
                          << desc.displayName() << "] \"" << name << "\"\n";
                break;
            }
            ParameterPrompter::printErrors(result.validation);
            current = std::move(candidate);
        } catch (const std::exception& e) {
            std::cout << "\n" << Terminal::indent << "Error: " << e.what() << "\n";
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
        std::cout << Terminal::indent << "Node " << id << " (\"" << name << "\") removed.\n";
    } catch (const std::exception& e) {
        std::cout << Terminal::indent << "Error: " << e.what() << "\n";
    }
}

void PipelineController::connectNodes() {
    Terminal::header("Connect Nodes");
    printNodeTable();
    if (model.nodeCount() < 2) {
        std::cout << Terminal::indent << "Need at least two nodes to connect.\n";
        return;
    }

    std::cout << "\n";
    NodeId from;
    NodeId to;
    if (!askNodeId("From node ID", from)) {
        return;
    }
    if (!askNodeId("To node ID", to)) {
        return;
    }

    try {
        model.connect(from, to);
        std::cout << Terminal::indent << "Connected: "
                  << from << " [" << model.node(from).displayName << "]"
                  << "  -->  "
                  << to   << " [" << model.node(to).displayName   << "]\n";
    } catch (const std::exception& e) {
        std::cout << Terminal::indent << "Error: " << e.what() << "\n";
    }
}

void PipelineController::disconnectNodes() {
    Terminal::header("Disconnect Nodes");
    printConnectionList();
    if (model.getConnections().empty()) {
        return;
    }

    std::cout << "\n";
    NodeId from;
    NodeId to;
    if (!askNodeId("From node ID", from)) {
        return;
    }
    if (!askNodeId("To node ID", to)) {
        return;
    }

    try {
        model.disconnect(from, to);
        std::cout << Terminal::indent << "Disconnected: " << from << " --> " << to << "\n";
    } catch (const std::exception& e) {
        std::cout << Terminal::indent << "Error: " << e.what() << "\n";
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
        std::cout << Terminal::indent << "[" << desc.displayName()
                  << "] has no configurable parameters.\n";
        return;
    }

    std::cout << "\n" << Terminal::indent << "Node " << id << ": [" << desc.displayName()
              << "] \"" << info.displayName << "\"\n"
              << Terminal::indent << "Current parameters:\n";
    ParameterPrompter::print(info.params);
    std::cout << "\n" << Terminal::indent << "New parameters (press Enter to keep current value):\n";

    NodeParams current{ info.params };
    while (true) {
        NodeParams candidate{ ParameterPrompter::prompt(current) };

        try {
            const ParameterValidator::ValidationResult result{ model.configureNode(id, candidate) };
            if (result.ok()) {
                std::cout << "\n" << Terminal::indent << "Parameters updated.\n";
                break;
            }
            ParameterPrompter::printErrors(result);
            current = std::move(candidate);
        } catch (const std::exception& e) {
            std::cout << Terminal::indent << "Error: " << e.what() << "\n";
            break;
        }
    }
}

void PipelineController::listPipeline() const {
    Terminal::header("Pipeline Overview");
    const std::string& in{ model.getInputPath() };
    const std::string& out{ model.getOutputPath() };
    std::cout << Terminal::indent << "Input  : " << (in.empty()  ? "(not set)" : in)  << "\n";
    std::cout << Terminal::indent << "Output : " << (out.empty() ? "(not set)" : out) << "\n\n";
    std::cout << Terminal::indent << "Nodes:\n";
    printNodeTable();
    std::cout << "\n" << Terminal::indent << "Connections:\n";
    printConnectionList();
}

void PipelineController::run() {
    Terminal::header("Run Pipeline");

    try {
        std::cout << Terminal::indent << "Loading: " << model.getInputPath() << "\n";
        std::cout << Terminal::indent << "Executing pipeline...\n";

        PipelineModel::RunResult result{ model.execute() };

        std::cout << Terminal::indent << "Done in " << std::fixed << std::setprecision(3)
                  << result.elapsedSeconds << " s\n\n";
        std::cout << Terminal::indent << "Saving " << result.outputs.size() << " output(s):\n";

        const std::string& output_path{ model.getOutputPath() };
        const size_t dot_pos{ output_path.find_last_of('.') };
        const std::string base{
            (dot_pos == std::string::npos) ? output_path : output_path.substr(0, dot_pos)
        };
        const std::string ext{
            (dot_pos == std::string::npos) ? std::string{ ".png" } : output_path.substr(dot_pos)
        };

        for (auto& [id, ctx] : result.outputs) {
            const std::string label{ ctx.getAppliedComponents() };
            const std::string current_base{ base + "_" + std::to_string(id) };

            const auto make_name{ [&](const char* suffix) {
                return std::string(current_base)
                    .append("_")
                    .append(suffix)
                    .append("_")
                    .append(label);
            } };

            ctx.save(
                make_name("output"), ext
            );
            ctx.getProcessedImage().save(
                make_name("processed"), ext
            );
            ctx.getEdgeMap().save(
                make_name("edge"), ext
            );
            ctx.getShapeMap().save(
                make_name("shape"), ext
            );
            std::cout << Terminal::indent << Terminal::indent << "[sink " << id << "] "
                      << make_name("output") << ext << "\n";
        }

    } catch (const std::exception& e) {
        std::cout << Terminal::indent << "Error: " << e.what() << "\n";
    }
}

bool PipelineController::askNodeId(const std::string& prompt, NodeId& out) const {
    static constexpr int cancel_value{ -1 };
    while (true) {
        const int v{ Terminal::readNodeId(prompt, cancel_value) };
        if (v == cancel_value) {
            return false;
        }
        if (v >= 0) {
            const NodeId id{ static_cast<NodeId>(v) };
            if (model.hasNode(id)) {
                out = id;
                return true;
            }
            std::cout << Terminal::indent << "Node " << v << " does not exist. Try again.\n";
        } else {
            std::cout << Terminal::indent << "Invalid ID.\n";
        }
    }
}

void PipelineController::printNodeTable() const {
    if (model.nodeCount() == 0) {
        std::cout << Terminal::indent << "(no nodes)\n";
        return;
    }

    std::cout << Terminal::indent << std::left
              << std::setw(Terminal::id_col_width)   << "ID"
              << std::setw(Terminal::type_col_width) << "Type"
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
        std::cout << Terminal::indent << std::left
                  << std::setw(Terminal::id_col_width)   << id
                  << std::setw(Terminal::type_col_width) << desc.displayName()
                  << info.displayName << "\n";
    }
}

void PipelineController::printConnectionList() const {
    const std::vector<pipeline::Connection>& connections{ model.getConnections() };
    if (connections.empty()) {
        std::cout << Terminal::indent << "(no connections)\n";
        return;
    }
    for (const auto& [from, to] : connections) {
        std::cout << Terminal::indent << from << " [" << model.node(from).displayName << "]"
                  << "  -->  "
                  << to   << " [" << model.node(to).displayName   << "]\n";
    }
}
