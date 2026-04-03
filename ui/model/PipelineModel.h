#ifndef PIPELINE_MODEL_H
#define PIPELINE_MODEL_H

#include "NodeTypes.h"
#include "ComponentRegistry.h"
#include "ParameterValidator.h"
#include "Pipeline.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class PipelineModel {
public:
    explicit PipelineModel(std::string inputPath = "", std::string outputPath = "");

    const std::string& getInputPath()  const { return inputPath; }
    const std::string& getOutputPath() const { return outputPath; }
    void setInputPath (std::string path)  { inputPath  = std::move(path); }
    void setOutputPath(std::string path) { outputPath = std::move(path); }

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
    std::string                          inputPath;
    std::string                          outputPath;

    static ParameterValidator::ValidationResult validateParams(const NodeParams& params);
};

#endif // PIPELINE_MODEL_H
