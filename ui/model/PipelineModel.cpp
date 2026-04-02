#include "PipelineModel.h"

#include "Image.h"
#include "Context.h"

#include <algorithm>
#include <chrono>
#include <stdexcept>

PipelineModel::PipelineModel(std::string inputPath, std::string outputPath)
    : inputPath(std::move(inputPath))
    , outputPath(std::move(outputPath)) {}

NodeId PipelineModel::addNode(
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

void PipelineModel::removeNode(NodeId id) {
    pipeline.removeNode(id);
    nodeInfo.erase(id);
}

void PipelineModel::configureNode(NodeId id, NodeParams newParams) {
    std::visit([&](const auto& p) {
        using T = std::decay_t<decltype(p)>;
        if constexpr (!std::is_same_v<T, std::monostate>) {
            pipeline.getComponent(id).setParameters(p);
        }
    }, newParams);
    nodeInfo.at(id).params = std::move(newParams);
}

void PipelineModel::connect(NodeId from, NodeId to) {
    pipeline.connect(from, to);
}

void PipelineModel::disconnect(NodeId from, NodeId to) {
    pipeline.disconnect(from, to);
}

PipelineModel::RunResult PipelineModel::execute() {
    if (inputPath.empty()) {
        throw std::runtime_error{ "Input path not set." };
    }
    if (outputPath.empty()) {
        throw std::runtime_error{ "Output path not set." };
    }
    if (nodeInfo.empty()) {
        throw std::runtime_error{ "Pipeline has no nodes." };
    }

    Image image{ inputPath };
    components::Context context{ image };

    const auto start  { std::chrono::high_resolution_clock::now() };
          auto outputs{ pipeline.execute(context) };
    const auto end    { std::chrono::high_resolution_clock::now() };

    const float elapsed{ std::chrono::duration<float>(end - start).count() };

    return RunResult{ std::move(outputs), elapsed };
}
