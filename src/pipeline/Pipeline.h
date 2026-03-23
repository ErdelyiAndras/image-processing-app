#ifndef PIPELINE_H
#define PIPELINE_H

#include "Component.h"
#include "Context.h"
#include "types.h"
#include "MergeStrategy.h"

#include <vector>
#include <memory>
#include <typeinfo>
#include <variant>
#include <cassert>
#include <unordered_map>

namespace pipeline {
    class Pipeline {
    public:
        NodeId addNode(std::unique_ptr<components::Component> component);
        NodeId addNode(std::unique_ptr<MergeStrategy> mergeStrategy);
        void   connect(NodeId from, NodeId to);

        void removeNode(NodeId id);
        void disconnect(NodeId from, NodeId to);

        components::Component& getComponent(NodeId nodeId);
        template <typename T>
        T& getComponentAs(NodeId nodeId) {
            T* component{ dynamic_cast<T*>(&getComponent(nodeId)) };
            if (!component) {
                throw std::bad_cast{};
            }
            return *component;
        }

        bool validate();

        std::unordered_map<NodeId, components::Context> execute(components::Context context);

    private:
        struct ProcessingNode {
            std::unique_ptr<components::Component> component;
        };

        struct MergeNode {
            std::unique_ptr<MergeStrategy> mergeStrategy;
        };

        struct Node {
            std::variant<ProcessingNode, MergeNode> data;
            std::vector<NodeId> successors;
            std::vector<NodeId> predecessors;
            bool removed{ false };
        };

        struct NodeResetter {
            void operator()(ProcessingNode& n) const {
                n.component.reset();
            }
            void operator()(MergeNode& n) const {
                n.mergeStrategy.reset();
            }
        };

        struct NodeMerger {
            std::vector<components::Context>& pending;

            components::Context operator()(const ProcessingNode&) const {
                assert(pending.size() == 1U);
                return std::move(pending.back());
            }

            components::Context operator()(const MergeNode& n) const {
                assert(!pending.empty());
                return n.mergeStrategy->merge(pending);
            }
        };

        struct NodeProcessor {
            components::Context& context;

            void operator()(const ProcessingNode& n) const {
                n.component->process(context);
            }

            void operator()(const MergeNode&) const {}
        };

        enum class Color : uint8_t  {
            Unvisited = 0U,
            OnStack   = 1U,
            Done      = 2U
        };

        bool isValid{ true };
        std::vector<Node> nodes;

        void assertNodeExists(NodeId id, const char* caller) const;

        std::vector<NodeId> sources() const;
        std::vector<NodeId> sinks()   const;
        std::vector<NodeId> topologicalSort() const;
        bool hasCycle() const;
        bool hasNodeWithInvalidPredecessors() const;
        bool dfs(NodeId id, std::vector<Color>& color) const;
    };
} // namespace pipeline

#endif // PIPELINE_H
