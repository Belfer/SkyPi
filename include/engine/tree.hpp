#pragma once

#include <engine/byte_types.hpp>
#include <engine/macros.hpp>
#include <engine/list.hpp>
#include <engine/hash_map.hpp>

#include <functional>

using TreeNodeId = SizeType;
constexpr TreeNodeId INVALID_TREENODE_ID = 0;

template <typename T>
class TreeNode
{
public:
    TreeNode() {}
    TreeNode(const T& data)
        : data(data)
        , parent(INVALID_TREENODE_ID)
    {}

    T data;
    TreeNodeId parent;
    List<TreeNodeId> children;
};

template <typename T>
class Tree
{
public:
    using Node = TreeNode<T>;
    using RecurseFn = std::function<void(TreeNodeId, const Node&)>;

    template <typename C>
    using RecurseParamFn = std::function<C(const C&, TreeNodeId, const Node&)>;

public:
    Tree() {}

    inline TreeNodeId GetRoot() const { return m_root; }

    inline bool IsValid(TreeNodeId nodeId) const
    {
        auto it = m_indices.find(nodeId);
        return it != m_indices.end();
    }

    inline TreeNodeId CreateNode(const T& data)
    {
        static std::hash<T> hashFn;
        TreeNodeId nodeId = hashFn(data);

        m_nodes.emplace_back(data);
        m_indices.insert(std::make_pair(nodeId, m_nodes.size() - 1));

        if (m_root == INVALID_TREENODE_ID)
            m_root = nodeId;

        return nodeId;
    }

    inline const Node& GetNode(TreeNodeId nodeId) const
    {
        auto nodeIndex = GetIndex(nodeId);
        ENSURE(nodeIndex >= 0 || nodeIndex < m_nodes.size());
        return m_nodes[nodeIndex];
    }

    inline void AddChild(TreeNodeId parentId, TreeNodeId childId)
    {
        auto parentIndex = GetIndex(parentId);
        auto childIndex = GetIndex(childId);

        ENSURE(parentIndex >= 0 || parentIndex < m_nodes.size());
        ENSURE(childIndex >= 0 || childIndex < m_nodes.size());

        m_nodes[parentIndex].children.emplace_back(childId);
        m_nodes[childIndex].parent = parentId;
    }

    inline void Clear()
    {
        m_root = INVALID_TREENODE_ID;
        m_nodes.clear();
        m_indices.clear();
    }

    inline void Recurse(TreeNodeId nodeId, const RecurseFn& recurseFn) const
    {
        const auto& node = GetNode(nodeId);
        recurseFn(nodeId, node);

        for (TreeNodeId childIndex : node.children)
        {
            Recurse(childIndex, recurseFn);
        }
    }

    template <typename C>
    inline void RecurseParam(const C& param, TreeNodeId nodeId, const RecurseParamFn<C>& recurseFn) const
    {
        const auto& node = GetNode(nodeId);
        C ret = recurseFn(param, nodeId, node);

        for (TreeNodeId childIndex : node.children)
        {
            RecurseParam(ret, childIndex, recurseFn);
        }
    }

private:
    inline SizeType GetIndex(TreeNodeId nodeId) const
    {
        auto it = m_indices.find(nodeId);
        ENSURE(it != m_indices.end());
        return it->second;
    }

private:
    template <typename T2>
    friend class Serial;

    TreeNodeId m_root = INVALID_TREENODE_ID;

    List<Node> m_nodes;
    HashMap<TreeNodeId, SizeType> m_indices;
};