#include "PipelineModel.h"

#include "Component.h"
#include "ComponentRegistry.h"
#include "Context.h"
#include "Image.h"
#include "NodeTypes.h"
#include "ParameterValidator.h"
#include "types.h"

#include <chrono>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>

void PipelineModel::setInputPath(const std::string& path) {
    std::filesystem::path path_obj{ path };
    validateInputPath(path_obj);
    inputPath = std::move(path_obj);
}

void PipelineModel::setOutputPath(const std::string& path) {
    std::filesystem::path path_obj{ path };
    validateOutputPath(path_obj);
    outputPath = std::move(path_obj);
}

PipelineModel::AddNodeResult PipelineModel::addNode(
    const ComponentDescriptor& desc, NodeParams params, const std::string& name
) {
    ParameterValidator::ValidationResult validation{ validateParams(params) };
    if (!validation.ok()) {
        return AddNodeResult{ std::move(validation), std::nullopt };
    }

    NodeId id;
    if (desc.isMerge()) {
        id = pipeline.addNode(desc.makeMerge());
    } else {
        id = pipeline.addNode(desc.makeComponent(params));
    }
    nodeInfo.insert_or_assign(id, NodeInfo{ desc.type(), name, std::move(params) });
    return AddNodeResult{ std::move(validation), id };
}

void PipelineModel::removeNode(NodeId id) {
    pipeline.removeNode(id);
    nodeInfo.erase(id);
}

ParameterValidator::ValidationResult PipelineModel::configureNode(NodeId id, NodeParams newParams) {
    ParameterValidator::ValidationResult result{ validateParams(newParams) };
    if (!result.ok()) {
        return result;
    }

    std::visit([&](const auto& p) {
        using T = std::decay_t<decltype(p)>;
        if constexpr (!std::is_same_v<T, std::monostate>) {
            pipeline.getComponent(id).setParameters(p);
        }
    }, newParams);
    nodeInfo.at(id).params = std::move(newParams);
    return result;
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

    Image image{ inputPath.string() };
    const components::Context context{ image };

    const auto start  { std::chrono::high_resolution_clock::now() };
    std::unordered_map<NodeId, components::Context> outputs{ pipeline.execute(context) };
    const auto end    { std::chrono::high_resolution_clock::now() };

    const float elapsed{ std::chrono::duration<float>(end - start).count() };

    return RunResult{ std::move(outputs), elapsed };
}

ParameterValidator::ValidationResult PipelineModel::validateParams(const NodeParams& params) {
    return std::visit([](const auto& p) {
        using T = std::decay_t<decltype(p)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            return ParameterValidator::ValidationResult{};
        } else {
            return ParameterValidator::validate(p);
        }
    }, params);
}

void PipelineModel::validateInputPath(const std::filesystem::path& path) {
    if (path.empty()) {
        throw std::invalid_argument{ "Input path cannot be empty." };
    }
    if (!path.has_filename()) {
        throw std::invalid_argument{ "Input path must include a file name." };
    }
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
        throw std::invalid_argument{ "Input path does not point to an existing file: " + path.string() };
    }

}

void PipelineModel::validateOutputPath(const std::filesystem::path& path) {
    if (path.empty()) {
        throw std::invalid_argument{ "Output path cannot be empty." };
    }
    if (!path.has_filename()) {
        throw std::invalid_argument{ "Output path must include a file name." };
    }
    if (!path.has_extension()) {
        throw std::invalid_argument{ "Output path must include a file extension." };
    }
    const std::string ext{ path.extension().string() };
    if (!Image::isSupportedSaveExtension(ext)) {
        throw std::invalid_argument{
            "Unsupported output extension: " + ext + " (expected .png, .jpg, or .jpeg)."
        };
    }
    if (!path.parent_path().empty() && !std::filesystem::exists(path.parent_path())) {
        throw std::invalid_argument{ "Output path's parent directory does not exist: " + path.parent_path().string() };
    }
}
