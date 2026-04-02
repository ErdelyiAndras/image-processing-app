#include "PipelineController.h"

#include "Image.h"
#include "Context.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <stdexcept>

PipelineController::PipelineController(std::string inputPath, std::string outputPath)
    : inputPath(std::move(inputPath))
    , outputPath(std::move(outputPath)) {}

void PipelineController::setInput() {
    Terminal::header("Set Input Image");
    const std::string path{ Terminal::readString("Image path", inputPath) };
    if (!path.empty()) {
        inputPath = path;
        std::cout << "\n  Input set to: " << inputPath << "\n";
    } else {
        std::cout << "  No change.\n";
    }
}

void PipelineController::setOutput() {
    Terminal::header("Set Output Path");
    std::cout << "  Include extension, e.g. output/result.png\n\n";
    const std::string path{ Terminal::readString("Output path", outputPath) };
    if (!path.empty()) {
        outputPath = path;
        std::cout << "\n  Output set to: " << outputPath << "\n";
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

    const int cat{ Terminal::readChoice(0, static_cast<uint8_t>(Category::CategoryCount)) };
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

    NodeParams params{ std::monostate{} };
    if (desc.hasParams()) {
        std::cout << "\n  Parameters (press Enter to accept default):\n";
        params = desc.promptParams(desc.defaultParams());
    }

    std::cout << "\n";
    const std::string name{ Terminal::readString("Node name", desc.displayName()) };

    try {
        const NodeId id{ registerNode(desc, std::move(params), name) };
        std::cout << "\n  Node " << id << " added: ["
                  << desc.displayName() << "] \"" << name << "\"\n";
    } catch (const std::exception& e) {
        std::cout << "\n  Error: " << e.what() << "\n";
    }
}

void PipelineController::removeNode() {
    Terminal::header("Remove Node");
    printNodeTable();
    if (nodeInfo.empty()) {
        return;
    }

    std::cout << "\n";
    NodeId id;
    if (!askNodeId("Node ID to remove", id)) {
        return;
    }

    try {
        pipeline.removeNode(id);
        eraseConnectionsFor(id);
        const std::string name{ nodeInfo.at(id).displayName };
        nodeInfo.erase(id);
        std::cout << "  Node " << id << " (\"" << name << "\") removed.\n";
    } catch (const std::exception& e) {
        std::cout << "  Error: " << e.what() << "\n";
    }
}

void PipelineController::connectNodes() {
    Terminal::header("Connect Nodes");
    printNodeTable();
    if (nodeInfo.size() < 2) {
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
        pipeline.connect(from, to);
        connections.emplace_back(from, to);
        std::cout << "  Connected: "
                  << from << " [" << nodeInfo.at(from).displayName << "]"
                  << "  -->  "
                  << to   << " [" << nodeInfo.at(to).displayName   << "]\n";
    } catch (const std::exception& e) {
        std::cout << "  Error: " << e.what() << "\n";
    }
}

void PipelineController::disconnectNodes() {
    Terminal::header("Disconnect Nodes");
    printConnectionList();
    if (connections.empty()) {
        return;
    }

    std::cout << "\n";
    NodeId from, to;
    if (!askNodeId("From node ID", from)) {
        return;
    }
    if (!askNodeId("To node ID",   to)) {
        return;
    }

    try {
        pipeline.disconnect(from, to);
        connections.erase(
            std::remove_if(connections.begin(), connections.end(),
                [from, to](const Connection& c) {
                    return c.first == from && c.second == to;
                }),
            connections.end()
        );
        std::cout << "  Disconnected: " << from << " --> " << to << "\n";
    } catch (const std::exception& e) {
        std::cout << "  Error: " << e.what() << "\n";
    }
}

void PipelineController::configureNode() {
    Terminal::header("Configure Node");
    printNodeTable();
    if (nodeInfo.empty()) {
        return;
    }

    std::cout << "\n";
    NodeId id;
    if (!askNodeId("Node ID to configure", id)) {
        return;
    }

    NodeInfo& info{ nodeInfo.at(id) };
    const ComponentDescriptor& desc{ ComponentRegistry::get(info.type) };

    if (!desc.hasParams()) {
        std::cout << "  [" << desc.displayName()
                  << "] has no configurable parameters.\n";
        return;
    }

    std::cout << "\n  Node " << id << ": [" << desc.displayName()
              << "] \"" << info.displayName << "\"\n"
              << "  Current parameters:\n";
    desc.printParams(info.params);
    std::cout << "\n  New parameters (press Enter to keep current value):\n";

    NodeParams newParams{ desc.promptParams(info.params) };

    try {
        applyParamsToNode(id, newParams);
        info.params = std::move(newParams);
        std::cout << "\n  Parameters updated.\n";
    } catch (const std::exception& e) {
        std::cout << "  Error: " << e.what() << "\n";
    }
}

void PipelineController::listPipeline() const {
    Terminal::header("Pipeline Overview");
    std::cout << "  Input  : " << pathOrUnset(inputPath)  << "\n";
    std::cout << "  Output : " << pathOrUnset(outputPath) << "\n\n";
    std::cout << "  Nodes:\n";
    printNodeTable();
    std::cout << "\n  Connections:\n";
    printConnectionList();
}

void PipelineController::run() {
    Terminal::header("Run Pipeline");

    if (inputPath.empty()) {
        std::cout << "  Error: input path not set. Use option 1.\n";
        return;
    }
    if (outputPath.empty()) {
        std::cout << "  Error: output path not set. Use option 2.\n";
        return;
    }
    if (nodeInfo.empty()) {
        std::cout << "  Error: pipeline has no nodes. Use option 3.\n";
        return;
    }

    const size_t dotPos{ outputPath.find_last_of('.') };
    const std::string base{
        (dotPos == std::string::npos) ? outputPath : outputPath.substr(0, dotPos)
    };
    const std::string ext{
        (dotPos == std::string::npos) ? std::string{ ".png" } : outputPath.substr(dotPos)
    };

    try {
        std::cout << "  Loading: " << inputPath << "\n";
        Image image{ inputPath };
        components::Context context{ image };

        std::cout << "  Executing pipeline...\n";
        const auto start  { std::chrono::high_resolution_clock::now() };
              auto results{ pipeline.execute(context) };
        const auto end    { std::chrono::high_resolution_clock::now() };

        const float elapsed{ std::chrono::duration<float>(end - start).count() };
        std::cout << "  Done in " << std::fixed << std::setprecision(3)
                  << elapsed << " s\n\n";
        std::cout << "  Saving " << results.size() << " output(s):\n";

        for (auto& [id, ctx] : results) {
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
                makeName("map"), ext
            );
            std::cout << "    [sink " << id << "] "
                      << makeName("output") << ext << "\n";
        }

    } catch (const std::exception& e) {
        std::cout << "  Error: " << e.what() << "\n";
    }
}

std::string PipelineController::pathOrUnset(const std::string& s) {
    return s.empty() ? "(not set)" : s;
}

NodeId PipelineController::registerNode(
    const ComponentDescriptor& desc, NodeParams params, const std::string& name
) {
    NodeId id;
    if (desc.isMerge()) {
        id = pipeline.addNode(desc.makeMerge());
    } else {
        id = pipeline.addNode(desc.makeComponent(params));
    }
    nodeInfo.insert_or_assign(id, NodeInfo{ desc.type(), name, std::move(params) });
    return id;
}

void PipelineController::applyParamsToNode(NodeId id, const NodeParams& params) {
    std::visit([&](const auto& p) {
        using T = std::decay_t<decltype(p)>;
        if constexpr (!std::is_same_v<T, std::monostate>) {
            pipeline.getComponent(id).setParameters(p);
        }
    }, params);
}

void PipelineController::eraseConnectionsFor(NodeId id) {
    connections.erase(
        std::remove_if(connections.begin(), connections.end(),
            [id](const Connection& c) {
                return c.first == id || c.second == id;
            }
        ),
        connections.end()
    );
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
            if (nodeInfo.count(id)) {
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
    if (nodeInfo.empty()) {
        std::cout << "  (no nodes)\n";
        return;
    }

    std::cout << "  " << std::left
              << std::setw(5)  << "ID"
              << std::setw(32) << "Type"
              << "Name\n";
    Terminal::separator();

    std::vector<NodeId> ids;
    ids.reserve(nodeInfo.size());
    for (const auto& [key, _] : nodeInfo) {
        ids.push_back(key);
    }
    std::sort(ids.begin(), ids.end());

    for (const NodeId id : ids) {
        const NodeInfo& info{ nodeInfo.at(id) };
        const ComponentDescriptor& desc{ ComponentRegistry::get(info.type) };
        std::cout << "  " << std::left
                  << std::setw(5)  << id
                  << std::setw(32) << desc.displayName()
                  << info.displayName << "\n";
    }
}

void PipelineController::printConnectionList() const {
    if (connections.empty()) {
        std::cout << "  (no connections)\n";
        return;
    }
    for (const auto& [from, to] : connections) {
        std::cout << "  " << from << " [" << nodeInfo.at(from).displayName << "]"
                  << "  -->  "
                  << to   << " [" << nodeInfo.at(to).displayName   << "]\n";
    }
}
