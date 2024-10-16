#ifndef MTREENODES_HPP
#define MTREENODES_HPP

#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream> // Para std::ostream
#include <limits>   // Para std::numeric_limits
#include "TreeObject.hpp"
#include "debug_msg.hpp"

extern size_t maxNodeId;

/**
 * @brief Classe base para um nó na M-Tree.
 *
 * @tparam T O tipo dos elementos armazenados na M-Tree.
 */
template <typename T>
class Node : public std::enable_shared_from_this<Node<T>>
{
public:
    typedef std::shared_ptr<TreeObject<T>> TreeObjectPtr;
    typedef std::shared_ptr<Node<T>> NodePtr;

    Node(size_t nodeId, size_t maxCapacity, bool isRoot = false, NodePtr parentNode = nullptr, TreeObjectPtr parentRoutingObj = nullptr)
        : nodeId(nodeId), maxCapacity(maxCapacity), isRoot(isRoot), parentNode(parentNode), parentRoutingObj(parentRoutingObj) {}
    virtual ~Node() = default;

    virtual void insert(const T &element, std::function<double(const T &, const T &)> distance) = 0;
    virtual bool isLeaf() const = 0;

    size_t getNodeId() const { return nodeId; }
    bool getIsRoot() const { return isRoot; }
    void setIsRoot(bool root) { isRoot = root; }
    NodePtr getParentNode() const { return parentNode; }
    void setParentNode(NodePtr parentNode) { this->parentNode = parentNode; }
    TreeObjectPtr getParentRoutingObj() const { return parentRoutingObj; }
    void setParentRoutingObj(TreeObjectPtr parentRoutingObj) { this->parentRoutingObj = parentRoutingObj; }
    std::vector<TreeObjectPtr>& getEntries() { return entries; }
    const std::vector<TreeObjectPtr>& getEntries() const { return entries; }
    virtual NodePtr createNewNode(size_t nodeId) const = 0;
    virtual NodePtr createNewRootNode(size_t nodeId) const = 0;
    
    virtual void updateRoutingObject(TreeObjectPtr p, std::vector<TreeObjectPtr> entries, std::shared_ptr<Node<T>> childNode, std::shared_ptr<Node<T>> parentNode, std::function<double(const T &, const T &)> distance) = 0;
    virtual void getRepr(std::ostream &os) const = 0;

    // Overload of output stream operator
    friend std::ostream &operator<<(std::ostream &os, const Node<T> &node) {

        for (size_t i = 0; i < node.entries.size(); i++)
        {
            if (node.isRoot && i == 0)
            {
                os << "(Root)";
            }
            if (node.isLeaf()){
                os << "[" << node.entries[i]->getRepresentative() << "]";
            } else {
                os << "(" << node.entries[i]->getRepresentative() << ")";
            }
            // Check if null
            if (node.entries[i]->getSubtree() == nullptr)
            {
                os << "*";
            }
            os << " ";
        }

        // Recurse down the tree
        for (size_t i = 0; i < node.entries.size(); i++)
        {
            if (node.entries[i]->getSubtree() != nullptr)
            {
                os << "\n";
                node.entries[i]->getSubtree()->getRepr(os);
            }
        }
        return os;
    } 

    std::pair<TreeObjectPtr, TreeObjectPtr> promote() const
    {
        // Create a copy of the entries[0] and entries[1] as routing objects
        TreeObjectPtr p1 = std::make_shared<RoutingObject<T>>(entries[0]->getRepresentative(), entries[0]->getCoveringRadius(), entries[0]->getSubtree(), entries[0]->getDistanceToParent());
        TreeObjectPtr p2 = std::make_shared<RoutingObject<T>>(entries[1]->getRepresentative(), entries[1]->getCoveringRadius(), entries[1]->getSubtree(), entries[1]->getDistanceToParent());
        return std::make_pair(p1, p2);
    }

    void split(const TreeObjectPtr &entry, std::function<double(const T &, const T &)> distance)
    {
        // The new routing objects are now all those in the node plus the new routing object
        std::vector<TreeObjectPtr> allEntries = entries;
        allEntries.push_back(entry);

        // Create a new node N'
        // Maybe needs to update node ID and ParentNode after creating the new node
        maxNodeId = maxNodeId + 1;
        auto newNode = createNewNode(maxNodeId);

        DEBUG_MSG("Created new node " << newNode->getNodeId());

        // PROMOTE
        std::pair<TreeObjectPtr, TreeObjectPtr> promoted = promote();
        TreeObjectPtr p1 = promoted.first;
        TreeObjectPtr p2 = promoted.second;

        DEBUG_MSG("Promoted routing objects " << p1->getRepresentative() << " and " << p2->getRepresentative());

        // PARTITION
        // Creates a partition of the overflown node and inserts the entries in the
        // curent node and the new node
        size_t mid = allEntries.size() / 2;
        std::pair<std::vector<TreeObjectPtr>, std::vector<TreeObjectPtr>> partEntries = std::make_pair(std::vector<TreeObjectPtr>(allEntries.begin(), allEntries.begin() + mid), std::vector<TreeObjectPtr>(allEntries.begin() + mid, allEntries.end()));
        std::vector<TreeObjectPtr> entries1 = partEntries.first;
        std::vector<TreeObjectPtr> entries2 = partEntries.second;

        // Store entries in each new routing object
        this->entries.clear();
        storeEntries(entries1, this);
        storeEntries(entries2, newNode);

        DEBUG_MSG("Stored " << this->entries.size() << " entries in node " << this->getNodeId());
        for (const auto &entry : this->entries)
        {
            DEBUG_MSG("Entry " << entry->getRepresentative());
        }
        DEBUG_MSG("Stored " << newNode->entries.size() << " entries in node " << newNode->getNodeId());
        for (const auto &entry : newNode->entries)
        {
            DEBUG_MSG("Entry " << entry->getRepresentative());
        }

        if (isRoot)
        {
            // Create a new root node and store the new routing objects
            maxNodeId = maxNodeId + 1;
            auto newRoot = createNewRootNode(maxNodeId);
            newRoot->setIsRoot(true);
            DEBUG_MSG("Splitting in the root: create a level new root node " << newRoot->getNodeId());

            // For each new routing object (p1, p2), update coveringRadius, subtree and distanceToParent
            // The new covering radius of p1 is max{d(p, p1) | for p in entries1}
            updateRoutingObject(p1, entries1, this->shared_from_this(), newRoot, distance);

            updateRoutingObject(p2, entries2, newNode, newRoot, distance);

            DEBUG_MSG(nodeId << " was parented to " << parentNode->getNodeId());
            DEBUG_MSG(p1->getSubtree()->getNodeId() << " is linked to object " << p1->getRepresentative() << " with covering radius " << p1->getCoveringRadius());
            
            DEBUG_MSG(newNode->getNodeId() << " was parented to " << newNode->getParentNode()->getNodeId());
            DEBUG_MSG(p2->getSubtree()->getNodeId() << " is linked to object " << p2->getRepresentative() << " with covering radius " << p2->getCoveringRadius());

            // Reset flag of the old root
            isRoot = false;
        }
        else
        {
            // Replace the parentRoutingObj with p1
            for (auto &entry : parentNode->entries)
            {
                if (entry == parentRoutingObj)
                {
                    // Remove entry from parent node
                    parentNode->entries.erase(std::remove(parentNode->entries.begin(), parentNode->entries.end(), parentRoutingObj), parentNode->entries.end());
                    
                    // Store p1 in parent
                    updateRoutingObject(p1, entries1, this->shared_from_this(), parentNode, distance);

                    DEBUG_MSG(nodeId << " was parented to " << parentNode->getNodeId());
                    DEBUG_MSG(p1->getSubtree()->getNodeId() << " is linked to object " << p1->getRepresentative() << " with covering radius " << p1->getCoveringRadius());
                    break;
                }
            }

            // Check if parent node is full
            if (parentNode->entries.size() < maxCapacity)
            {
                // Store p2 in parent
                updateRoutingObject(p2, entries2, newNode, parentNode, distance);
                DEBUG_MSG(newNode->getNodeId() << " was parented to " << newNode->getParentNode()->getNodeId());
                DEBUG_MSG(p2->getSubtree()->getNodeId() << " is linked to object " << p2->getRepresentative() << " with covering radius " << p2->getCoveringRadius());
            }
            else
            {
                // Split parent node
                parentNode->split(p2, distance);
            }
        }
    }

protected:
    size_t nodeId;
    size_t maxCapacity;
    bool isRoot;
    std::vector<TreeObjectPtr> entries;
    NodePtr parentNode;
    TreeObjectPtr parentRoutingObj;

    // Overload for other
    void storeEntries(const std::vector<TreeObjectPtr> &newEntries, std::shared_ptr<Node<T>> node)
    {
        for (const auto &entry : newEntries)
        {
            node->entries.push_back(entry);
        }
    }

    // Overload for this
    // Quando é a própria classe que faz a chamada de store Entries, não consigo usar
    // storeEntries com shared_ptr
    void storeEntries(const std::vector<TreeObjectPtr> &newEntries, Node<T>* node)
    {
        for (const auto &entry : newEntries)
        {
            node->entries.push_back(entry);
        }
    }

};

/**
 * @brief Classe para um nó folha na M-Tree.
 *
 * @tparam T O tipo dos elementos armazenados na M-Tree.
 */
template <typename T>
class LeafNode : public Node<T>
{
public:
    typedef typename Node<T>::TreeObjectPtr TreeObjectPtr;
    typedef typename Node<T>::NodePtr NodePtr;

    LeafNode(size_t nodeId, size_t maxCapacity, bool isRoot,NodePtr parentNode, TreeObjectPtr parentRoutingObj);

    void insert(const T &element, std::function<double(const T &, const T &)> distance) override;

    bool isLeaf() const override;

    // Create New Node
    typename Node<T>::NodePtr createNewNode(size_t nodeId) const override;

    // Create New Root Node
    typename Node<T>::NodePtr createNewRootNode(size_t nodeId) const override;

    void updateRoutingObject(TreeObjectPtr p, std::vector<TreeObjectPtr> entries, std::shared_ptr<Node<T>> childNode, std::shared_ptr<Node<T>> parentNode, std::function<double(const T &, const T &)> distance) override;

    void getRepr(std::ostream &os) const override;
};

/**
 * @brief Classe para um nó interno na M-Tree.
 *
 * @tparam T O tipo dos elementos armazenados na M-Tree.
 */
template <typename T>
class InternalNode : public Node<T>
{
public:
    typedef typename Node<T>::TreeObjectPtr TreeObjectPtr;
    typedef typename Node<T>::NodePtr NodePtr;

    InternalNode(size_t nodeId, size_t maxCapacity, bool isRoot, NodePtr parentNode, TreeObjectPtr parentRoutingObj);

    void insert(const T &element, std::function<double(const T &, const T &)> distance) override;

    bool isLeaf() const override;

    // Create New Node
    typename Node<T>::NodePtr createNewNode(size_t nodeId) const override;

    // Create New Root Node
    typename Node<T>::NodePtr createNewRootNode(size_t nodeId) const override;

    void updateRoutingObject(TreeObjectPtr p, std::vector<TreeObjectPtr> entries, std::shared_ptr<Node<T>> childNode, std::shared_ptr<Node<T>> parentNode, std::function<double(const T &, const T &)> distance) override;

    void getRepr(std::ostream &os) const override;
};

#include "MTreeInternalNode.hpp"
#include "MTreeLeafNode.hpp"

#endif // MTREENODES_HPP