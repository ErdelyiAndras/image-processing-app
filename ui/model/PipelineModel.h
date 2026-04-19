#ifndef PIPELINE_MODEL_H
#define PIPELINE_MODEL_H

#include "ComponentRegistry.h"
#include "Context.h"
#include "NodeTypes.h"
#include "ParameterValidator.h"
#include "Pipeline.h"
#include "types.h"

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class PipelineModel {
public:
    std::string getInputPath () const { return inputPath.string(); }
    std::string getOutputPath() const { return outputPath.string(); }

    void setInputPath (const std::string& path);
    void setOutputPath(const std::string& path);

    size_t nodeCount()        const { return nodeInfo.size(); }
    bool   hasNode(NodeId id) const { return nodeInfo.count(id) > 0; }

    const std::unordered_map<NodeId, NodeInfo>& nodes()          const { return nodeInfo; }
    const NodeInfo&                             node(NodeId id)  const { return nodeInfo.at(id); }
    const std::vector<pipeline::Connection>&    getConnections() const { return pipeline.getConnections(); }

    struct AddNodeResult {
        ParameterValidator::ValidationResult validation;
        std::optional<NodeId> id;
    };

    AddNodeResult addNode(const ComponentDescriptor& desc, NodeParams params, const std::string& name);
    void          removeNode(NodeId id);

    ParameterValidator::ValidationResult configureNode(NodeId id, NodeParams newParams);

    void connect   (NodeId from, NodeId to);
    void disconnect(NodeId from, NodeId to);

    struct RunResult {
        std::unordered_map<NodeId, components::Context> outputs;
        float elapsedSeconds;
    };

    RunResult execute();

private:
    pipeline::Pipeline                   pipeline;
    std::unordered_map<NodeId, NodeInfo> nodeInfo;
    std::filesystem::path                inputPath;
    std::filesystem::path                outputPath;

    static ParameterValidator::ValidationResult validateParams(const NodeParams& params);

    static void validateInputPath (const std::filesystem::path& path);
    static void validateOutputPath(const std::filesystem::path& path);
};

#endif // PIPELINE_MODEL_H
