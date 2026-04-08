#include "Pipeline.h"

#include "Component.h"
#include "Context.h"
#include "MergeStrategy.h"
#include "types.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

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

        const std::vector<NodeId>::iterator succ_it{ std::find(nodes[from].successors.begin(), nodes[from].successors.end(), to) };
        if (succ_it != nodes[from].successors.end()) {
            throw std::invalid_argument{
                "Pipeline::connect: nodes " + std::to_string(from) + " and " + std::to_string(to) +
                " are already connected"
            };
        }

        isValid = false;
        nodes[from].successors.push_back(to);
        nodes[to].predecessors.push_back(from);
        connections.emplace_back(from, to);
    }

    void Pipeline::removeNode(NodeId id) {
        assertNodeExists(id, __func__);

        isValid = false;
        for (const NodeId pred : nodes[id].predecessors) {
            std::vector<NodeId>& succs{ nodes[pred].successors };
            succs.erase(std::remove(succs.begin(), succs.end(), id), succs.end());
            connections.erase(
                std::remove_if(connections.begin(), connections.end(),
                    [pred, id](const Connection& c) {
                        return c.first == pred && c.second == id;
                    }
                ),
                connections.end()
            );
        }
        for (const NodeId succ : nodes[id].successors) {
            std::vector<NodeId>& preds{ nodes[succ].predecessors };
            preds.erase(std::remove(preds.begin(), preds.end(), id), preds.end());
            connections.erase(
                std::remove_if(connections.begin(), connections.end(),
                    [id, succ](const Connection& c) {
                        return c.first == id && c.second == succ;
                    }
                ),
                connections.end()
            );
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
        const std::vector<NodeId>::iterator succ_it{ std::find(succs.begin(), succs.end(), to) };
        if (succ_it == succs.end()) {
            throw std::invalid_argument{
                "Pipeline::disconnect: no connection from node " + std::to_string(from) +
                " to node " + std::to_string(to)
            };
        }

        isValid = false;

        succs.erase(succ_it);

        std::vector<NodeId>& preds{ nodes[to].predecessors };
        const std::vector<NodeId>::iterator pred_it{ std::find(preds.begin(), preds.end(), from) };
        preds.erase(pred_it);

        connections.erase(
            std::remove_if(connections.begin(), connections.end(),
                [from, to](const Connection& c) {
                    return c.first == from && c.second == to;
                }
            ),
            connections.end()
        );
    }

    components::Component& Pipeline::getComponent(NodeId nodeId) const {
        assertNodeExists(nodeId, __func__);
        const ProcessingNode* proc_node{ std::get_if<ProcessingNode>(&nodes[nodeId].data) };
        if (proc_node == nullptr || !proc_node->component) {
            throw std::invalid_argument{
                "Pipeline::getComponent: node id " + std::to_string(nodeId) + " has no component"
            };
        }
        return *proc_node->component;
    }

    std::string Pipeline::getComponentName(NodeId nodeId) const {
        assertNodeExists(nodeId, __func__);
        if (std::holds_alternative<ProcessingNode>(nodes[nodeId].data)) {
            const ProcessingNode& proc_node{ std::get<ProcessingNode>(nodes[nodeId].data) };
            assert(proc_node.component);
            return proc_node.component->getName();
        }
        if (std::holds_alternative<MergeNode>(nodes[nodeId].data)) {
            const MergeNode& merge_node{ std::get<MergeNode>(nodes[nodeId].data) };
            assert(merge_node.mergeStrategy);
            return merge_node.mergeStrategy->getName();
        }

        throw std::logic_error{ "Pipeline::getComponentName: invalid node data variant" };
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

    void Pipeline::recompute() {
        if (!hasCycle() && !hasNodeWithInvalidPredecessors()) {
            computeSources();
            computeSinks();
            computeTopologicalOrder();
            isValid = true;
        }
    }

    bool Pipeline::validate() {
        if (!isValid) {
            recompute();
        }
        return isValid;
    }

    void Pipeline::computeTopologicalOrder() {
        std::vector<uint32_t> in_degree(nodes.size(), 0U);
        for (NodeId id{ 0U }; id < static_cast<NodeId>(nodes.size()); ++id) {
            if (nodes[id].removed) {
                continue;
            }
            for (const NodeId succ : nodes[id].successors) {
                ++in_degree[succ];
            }
        }

        std::queue<NodeId> q;
        for (NodeId id{ 0U }; id < static_cast<NodeId>(nodes.size()); ++id) {
            if (!nodes[id].removed && in_degree[id] == 0U) {
                q.push(id);
            }
        }

        topologicalOrder.clear();
        topologicalOrder.reserve(nodes.size());

        while (!q.empty()) {
            const NodeId id{ q.front() };
            q.pop();
            topologicalOrder.push_back(id);
            for (const NodeId succ : nodes[id].successors) {
                if (--in_degree[succ] == 0U) {
                    q.push(succ);
                }
            }
        }
    }

    void Pipeline::computeSources() {
        sources.clear();
        for (NodeId id{ 0U }; id < static_cast<NodeId>(nodes.size()); ++id) {
            if (!nodes[id].removed && nodes[id].predecessors.empty()) {
                assert(!std::holds_alternative<MergeNode>(nodes[id].data));
                sources.push_back(id);
            }
        }
    }

    void Pipeline::computeSinks() {
        sinks.clear();
        for (NodeId id{ 0U }; id < static_cast<NodeId>(nodes.size()); ++id) {
            if (!nodes[id].removed && nodes[id].successors.empty()) {
                sinks.push_back(id);
            }
        }
    }

    std::unordered_map<NodeId, components::Context> Pipeline::execute(components::Context context) {
        if (!validate()) {
            throw std::runtime_error{ "Pipeline::execute: graph contains a cycle or has node(s) with invalid predecessor count" };
        }

        std::unordered_map<NodeId, std::vector<components::Context>> pending;

        for (size_t i{ 0U }; i < sources.size(); ++i) {
            if (i + 1U < sources.size()) {
                pending[sources[i]].push_back(context);
            } else {
                pending[sources[i]].push_back(std::move(context));
            }
        }

        std::unordered_map<NodeId, components::Context> results;

        for (const NodeId id : topologicalOrder) {
            const Node& node{ nodes[id] };

            const std::unordered_map<NodeId, std::vector<components::Context>>::iterator it{ pending.find(id) };
            assert(it != pending.end() && !it->second.empty());

            components::Context node_context{ std::visit(NodeMerger{ it->second }, node.data) };
            pending.erase(it);

            std::visit(NodeProcessor{ node_context }, node.data);

            if (node.successors.empty()) {
                results.emplace(id, std::move(node_context));
            } else {
                for (size_t i{ 0U }; i < node.successors.size(); ++i) {
                    const NodeId succ{ node.successors[i] };
                    if (i + 1U < node.successors.size()) {
                        pending[succ].push_back(node_context);
                    } else {
                        pending[succ].push_back(std::move(node_context));
                    }
                }
            }
        }

        std::unordered_map<NodeId, components::Context> outputs;
        outputs.reserve(sinks.size());
        for (const NodeId sink : sinks) {
            outputs.emplace(sink, std::move(results.at(sink)));
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
