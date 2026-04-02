#ifndef PIPELINE_CONTROLLER_H
#define PIPELINE_CONTROLLER_H

#include "NodeTypes.h"
#include "ComponentRegistry.h"
#include "Terminal.h"
#include "Pipeline.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class PipelineController {
public:
    explicit PipelineController(std::string inputPath = "", std::string outputPath = "");

    const std::string& getInputPath()  const { return inputPath;  }
    const std::string& getOutputPath() const { return outputPath; }
    size_t             nodeCount()     const { return nodeInfo.size(); }

    void setInput();
    void setOutput();
    void addNode();
    void removeNode();
    void connectNodes();
    void disconnectNodes();
    void configureNode();
    void listPipeline() const;
    void run();

private:
    using Connection = std::pair<NodeId, NodeId>;

    pipeline::Pipeline                   pipeline;
    std::unordered_map<NodeId, NodeInfo> nodeInfo;
    std::vector<Connection>              connections;
    std::string                          inputPath;
    std::string                          outputPath;

    static std::string pathOrUnset(const std::string& s);

    NodeId registerNode(const ComponentDescriptor& desc, NodeParams params, const std::string& name);

    void applyParamsToNode(NodeId id, const NodeParams& params);

    void eraseConnectionsFor(NodeId id);

    bool askNodeId(const std::string& prompt, NodeId& out) const;

    void printNodeTable() const;
    void printConnectionList() const;
};

#endif // PIPELINE_CONTROLLER_H
