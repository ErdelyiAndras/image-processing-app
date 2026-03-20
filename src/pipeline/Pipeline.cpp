#include "Pipeline.h"

#include <functional>
#include <queue>
#include <string>
#include <unordered_map>

namespace pipeline {
    NodeId Pipeline::addNode(std::unique_ptr<components::Component> component) {
        isValid = false;
        const NodeId id{ static_cast<NodeId>(nodes.size()) };
        nodes.emplace_back(Node{ ProcessingNode{ std::move(component) }, {}, {}, false });
        return id;
    }

    NodeId Pipeline::addNode(std::unique_ptr<MergeStrategy> mergeStrategy) {
        isValid = false;
        const NodeId id{ static_cast<NodeId>(nodes.size()) };
        nodes.emplace_back(Node{ MergeNode{ std::move(mergeStrategy) }, {}, {}, false });
        return id;
    }

    void Pipeline::connect(NodeId from, NodeId to) {
        assertNodeExists(from, __func__);
        assertNodeExists(to, __func__);

        if (std::holds_alternative<ProcessingNode>(nodes[to].data) && !nodes[to].predecessors.empty()) {
            throw std::invalid_argument{
                "Pipeline::connect: processing node " + std::to_string(to) +
                " already has a predecessor (only merge nodes can have multiple predecessors)"
            };
        }

        std::vector<NodeId>::iterator succIt{ std::find(nodes[from].successors.begin(), nodes[from].successors.end(), to) };
        if (succIt != nodes[from].successors.end()) {
            throw std::invalid_argument{
                "Pipeline::connect: nodes " + std::to_string(from) + " and " + std::to_string(to) +
                " are already connected"
            };
        }

        isValid = false;
        nodes[from].successors.push_back(to);
        nodes[to].predecessors.push_back(from);
    }

    void Pipeline::removeNode(NodeId id) {
        assertNodeExists(id, __func__);

        isValid = false;
        for (const NodeId pred : nodes[id].predecessors) {
            std::vector<NodeId>& succs{ nodes[pred].successors };
            succs.erase(std::remove(succs.begin(), succs.end(), id), succs.end());
        }
        for (const NodeId succ : nodes[id].successors) {
            std::vector<NodeId>& preds{ nodes[succ].predecessors };
            preds.erase(std::remove(preds.begin(), preds.end(), id), preds.end());
        }
        std::visit(NodeResetter{}, nodes[id].data);
        nodes[id].predecessors.clear();
        nodes[id].successors.clear();
        nodes[id].removed = true;
    }

    void Pipeline::disconnect(NodeId from, NodeId to) {
        assertNodeExists(from, __func__);
        assertNodeExists(to, __func__);

        std::vector<NodeId>& succs{ nodes[from].successors };
        const std::vector<NodeId>::iterator succIt{ std::find(succs.begin(), succs.end(), to) };
        if (succIt == succs.end()) {
            throw std::invalid_argument{
                "Pipeline::disconnect: no connection from node " + std::to_string(from) +
                " to node " + std::to_string(to)
            };
        }

        isValid = false;

        succs.erase(succIt);

        std::vector<NodeId>& preds{ nodes[to].predecessors };
        const std::vector<NodeId>::iterator predIt{ std::find(preds.begin(), preds.end(), from) };
        preds.erase(predIt);
    }

    components::Component& Pipeline::getComponent(NodeId nodeId) {
        assertNodeExists(nodeId, __func__);
        ProcessingNode* procNode{ std::get_if<ProcessingNode>(&nodes[nodeId].data) };
        if (!procNode || !procNode->component) {
            throw std::invalid_argument{
                "Pipeline::getComponent: node id " + std::to_string(nodeId) + " has no component"
            };
        }
        return *procNode->component;
    }

    bool Pipeline::hasCycle() const {
        std::vector<Color> color(nodes.size(), Color::Unvisited);

        for (NodeId id{ 0U }; id < static_cast<NodeId>(nodes.size()); ++id) {
            if (!nodes[id].removed && color[id] == Color::Unvisited && dfs(id, color)) {
                return true;
            }
        }
        return false;
    }

    bool Pipeline::hasNodeWithInvalidPredecessors() const {
        for (NodeId id{ 0U }; id < static_cast<NodeId>(nodes.size()); ++id) {
            if (nodes[id].removed) {
                continue;
            }
            if (std::holds_alternative<ProcessingNode>(nodes[id].data) && nodes[id].predecessors.size() > 1U) {
                return true;
            }
            if (std::holds_alternative<MergeNode>(nodes[id].data) && nodes[id].predecessors.empty()) {
                return true;
            }
        }
        return false;
    }

    bool Pipeline::dfs(NodeId id, std::vector<Color>& color) const {
        color[id] = Color::OnStack;
        for (const NodeId succ : nodes[id].successors) {
            if (color[succ] == Color::OnStack) {
                return true;
            }
            if (color[succ] == Color::Unvisited && dfs(succ, color)) {
                return true;
            }
        }
        color[id] = Color::Done;
        return false;
    }

    bool Pipeline::validate() {
        if (!isValid) {
            isValid = !hasCycle() && !hasNodeWithInvalidPredecessors();
        }
        return isValid;
    }

    std::vector<NodeId> Pipeline::topologicalSort() const {
        std::vector<uint32_t> inDegree(nodes.size(), 0U);
        for (NodeId id{ 0U }; id < static_cast<NodeId>(nodes.size()); ++id) {
            if (nodes[id].removed) {
                continue;
            }
            for (const NodeId succ : nodes[id].successors) {
                ++inDegree[succ];
            }
        }

        std::queue<NodeId> q;
        for (NodeId id{ 0U }; id < static_cast<NodeId>(nodes.size()); ++id) {
            if (!nodes[id].removed && inDegree[id] == 0U) {
                q.push(id);
            }
        }

        std::vector<NodeId> order;
        order.reserve(nodes.size());

        while (!q.empty()) {
            const NodeId id{ q.front() };
            q.pop();
            order.push_back(id);
            for (const NodeId succ : nodes[id].successors) {
                if (--inDegree[succ] == 0U) {
                    q.push(succ);
                }
            }
        }

        return order;
    }

    std::vector<NodeId> Pipeline::sources() const {
        std::vector<NodeId> result;
        for (NodeId id{ 0U }; id < static_cast<NodeId>(nodes.size()); ++id) {
            if (!nodes[id].removed && nodes[id].predecessors.empty()) {
                assert(!std::holds_alternative<MergeNode>(nodes[id].data));
                result.push_back(id);
            }
        }
        return result;
    }

    std::vector<NodeId> Pipeline::sinks() const {
        std::vector<NodeId> result;
        for (NodeId id{ 0U }; id < static_cast<NodeId>(nodes.size()); ++id) {
            if (!nodes[id].removed && nodes[id].successors.empty()) {
                result.push_back(id);
            }
        }
        return result;
    }

    std::vector<components::Context> Pipeline::execute(components::Context context) {
        if (!validate()) {
            throw std::runtime_error{ "Pipeline::execute: graph contains a cycle" };
        }

        const std::vector<NodeId> order{ topologicalSort() };

        std::unordered_map<NodeId, std::vector<components::Context>> pending;

        const std::vector<NodeId> srcs{ sources() };
        for (size_t i{ 0U }; i < srcs.size(); ++i) {
            if (i + 1U < srcs.size()) {
                pending[srcs[i]].push_back(context);
            } else {
                pending[srcs[i]].push_back(std::move(context));
            }
        }

        std::unordered_map<NodeId, components::Context> results;

        for (const NodeId id : order) {
            const Node& node{ nodes[id] };

            std::unordered_map<NodeId, std::vector<components::Context>>::iterator it{ pending.find(id) };
            assert(it != pending.end() && !it->second.empty());

            components::Context nodeCtx{ std::visit(NodeMerger{ it->second }, node.data) };
            pending.erase(it);

            std::visit(NodeProcessor{ nodeCtx }, node.data);

            if (node.successors.empty()) {
                results.emplace(id, std::move(nodeCtx));
            } else {
                for (size_t i{ 0U }; i < node.successors.size(); ++i) {
                    const NodeId succ{ node.successors[i] };
                    if (i + 1U < node.successors.size()) {
                        pending[succ].push_back(nodeCtx);
                    } else {
                        pending[succ].push_back(std::move(nodeCtx));
                    }
                }
            }
        }

        const std::vector<NodeId> snks{ sinks() };
        std::vector<components::Context> outputs;
        outputs.reserve(snks.size());
        for (const NodeId sink : snks) {
            outputs.push_back(std::move(results.at(sink)));
        }
        return outputs;
    }

    void Pipeline::assertNodeExists(NodeId id, const char* caller) const {
        if (static_cast<size_t>(id) >= nodes.size()) {
            throw std::out_of_range{
                "Pipeline::" + std::string{ caller } + ": node id " + std::to_string(id) + " out of range"
            };
        }
        if (nodes[id].removed) {
            throw std::invalid_argument{
                "Pipeline::" + std::string{ caller } + ": node id " + std::to_string(id) + " has been removed"
            };
        }
    }
} // namespace pipeline
