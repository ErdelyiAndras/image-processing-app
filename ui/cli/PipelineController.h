#ifndef PIPELINE_CONTROLLER_H
#define PIPELINE_CONTROLLER_H

#include "PipelineModel.h"
#include "Terminal.h"

#include <string>

class PipelineController {
public:
    explicit PipelineController(std::string inputPath = "", std::string outputPath = "");

    const std::string& getInputPath()  const { return model.getInputPath();  }
    const std::string& getOutputPath() const { return model.getOutputPath(); }
    size_t             nodeCount()     const { return model.nodeCount(); }

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
    PipelineModel model;

    bool askNodeId(const std::string& prompt, NodeId& out) const;

    void printNodeTable() const;
    void printConnectionList() const;
};

#endif // PIPELINE_CONTROLLER_H
