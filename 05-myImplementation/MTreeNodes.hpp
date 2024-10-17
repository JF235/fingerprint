#ifndef MTREENODES_HPP
#define MTREENODES_HPP

#include <vector>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <iostream> // For std::ostream
#include <limits>   // For std::numeric_limits
#include "TreeObject.hpp"
#include "debug_msg.hpp"
#include "NNList.hpp"

extern size_t maxNodeId;

/**
 * @brief Base class for a node in the M-Tree.
 *
 * @tparam T The type of elements stored in the M-Tree.
 */
template <typename T>
class Node : public std::enable_shared_from_this<Node<T>>
{
public:
    typedef std::shared_ptr<TreeObject<T>> TreeObjectPtr;
    typedef std::shared_ptr<Node<T>> NodePtr;
    typedef std::function<double(const T &, const T &)> Metric;

    /**
     * @brief Constructs a Node.
     *
     * @param nodeId The unique ID of the node.
     * @param maxCapacity The maximum number of elements the node can hold.
     * @param isRoot Whether the node is the root node.
     * @param parentNode The parent node.
     * @param parentRoutingObj The parent routing object.
     */
    Node(size_t nodeId, size_t maxCapacity, bool isRoot = false, NodePtr parentNode = nullptr, TreeObjectPtr parentRoutingObj = nullptr)
        : nodeId(nodeId), maxCapacity(maxCapacity), isRoot(isRoot), parentNode(parentNode), parentRoutingObj(parentRoutingObj) {}
    virtual ~Node() = default;

    /**
     * @brief Inserts an element into the node.
     *
     * @param element The element to be inserted.
     * @param distance The distance function.
     */
    virtual void insert(const T &element, Metric distance) = 0;

    /**
     * @brief Gets the node ID.
     *
     * @return The node ID.
     */
    size_t getNodeId() const { return nodeId; }

    /**
     * @brief Checks if the node is the root.
     *
     * @return True if the node is the root, false otherwise.
     */
    bool getIsRoot() const { return isRoot; }

    /**
     * @brief Sets the node as root.
     *
     * @param root True to set the node as root, false otherwise.
     */
    void setIsRoot(bool root) { isRoot = root; }

    /**
     * @brief Gets the parent node.
     *
     * @return The parent node.
     */
    NodePtr getParentNode() const { return parentNode; }

    /**
     * @brief Sets the parent node.
     *
     * @param parentNode The parent node.
     */
    void setParentNode(NodePtr parentNode) { this->parentNode = parentNode; }

    /**
     * @brief Gets the parent routing object.
     *
     * @return The parent routing object.
     */
    TreeObjectPtr getParentRoutingObj() const { return parentRoutingObj; }

    /**
     * @brief Sets the parent routing object.
     *
     * @param parentRoutingObj The parent routing object.
     */
    void setParentRoutingObj(TreeObjectPtr parentRoutingObj) { this->parentRoutingObj = parentRoutingObj; }

    /**
     * @brief Gets the entries in the node.
     *
     * @return A reference to the vector of entries.
     */
    std::vector<TreeObjectPtr>& getEntries() { return entries; }

    /**
     * @brief Gets the entries in the node (const version).
     *
     * @return A const reference to the vector of entries.
     */
    const std::vector<TreeObjectPtr>& getEntries() const { return entries; }

    /**
     * @brief Checks if the node is a leaf.
     *
     * @return True if the node is a leaf, false otherwise.
     */
    bool getIsLeaf() const { return isLeaf; }

    /**
     * @brief Creates a new node.
     *
     * @param nodeId The unique ID of the new node.
     * @return A shared pointer to the new node.
     */
    virtual NodePtr createNewNode(size_t nodeId) const = 0;

    /**
     * @brief Creates a new root node.
     *
     * @param nodeId The unique ID of the new root node.
     * @return A shared pointer to the new root node.
     */
    virtual NodePtr createNewRootNode(size_t nodeId) const = 0;

    /**
     * @brief Searches for the nearest neighbors of an element in the node.
     *
     * @param element The element to search for.
     * @param nnList The list of nearest neighbors.
     * @param candidates The candidate nodes to search in.
     * @param distance The distance function.
     */
    virtual void search(const T &query, double dmin, NNList<T> &nnList, std::vector<std::pair<NodePtr, double>> &candidates, Metric distance) const = 0;

    /**
     * @brief Updates the routing object.
     *
     * @param p The routing object to be updated.
     * @param entries The entries in the node.
     * @param childNode The child node.
     * @param parentNode The parent node.
     * @param distance The distance function.
     */
    virtual void updateRoutingObject(TreeObjectPtr p, std::vector<TreeObjectPtr> entries, NodePtr childNode, NodePtr parentNode, Metric distance) = 0;

    /**
     * @brief Gets the string representation of the node.
     *
     * @param os The output stream.
     */
    virtual void getRepr(std::ostream &os) const = 0;

    /**
     * @brief Overloads the output stream operator to print the node.
     *
     * @param os The output stream.
     * @param node The node to be printed.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &os, const Node<T> &node) {
        for (size_t i = 0; i < node.entries.size(); i++) {
            if (node.isRoot && i == 0) {
                os << "(Root)";
            }
            if (node.getIsLeaf()) {
                os << "[" << node.entries[i]->getRepresentative() << "]";
            } else {
                os << "(" << node.entries[i]->getRepresentative() << ":" << node.entries[i]->getCoveringRadius() <<  ")";
            }
            if (node.entries[i]->getSubtree() == nullptr) {
                os << "*";
            }
            os << " ";
        }

        // Recurse down the tree
        for (size_t i = 0; i < node.entries.size(); i++) {
            if (node.entries[i]->getSubtree() != nullptr) {
                os << "\n";
                node.entries[i]->getSubtree()->getRepr(os);
            }
        }
        return os;
    }

    /**
     * @brief Promotes two entries to routing objects.
     *
     * @return A pair of routing objects.
     */
    std::pair<TreeObjectPtr, TreeObjectPtr> promote(std::vector<TreeObjectPtr> entries) const {
        TreeObjectPtr p1 = std::make_shared<RoutingObject<T>>(entries[0]->getRepresentative(), entries[0]->getCoveringRadius(), entries[0]->getSubtree(), entries[0]->getDistanceToParent());
        TreeObjectPtr p2 = std::make_shared<RoutingObject<T>>(entries[1]->getRepresentative(), entries[1]->getCoveringRadius(), entries[1]->getSubtree(), entries[1]->getDistanceToParent());
        return std::make_pair(p1, p2);
    }

    /**
     * @brief Partitions the entries that caused the node to split.
     *
     * @param entries The entries to be partitioned.
     * @param p1 The first routing object.
     * @param p2 The second routing object.
     * @return A pair of vectors of entries.
     */
    std::pair<std::vector<TreeObjectPtr>, std::vector<TreeObjectPtr>> partition(std::vector<TreeObjectPtr> entries, TreeObjectPtr p1, TreeObjectPtr p2) const {
        size_t mid = entries.size() / 2;
        return std::make_pair(std::vector<TreeObjectPtr>(entries.begin(), entries.begin() + mid), std::vector<TreeObjectPtr>(entries.begin() + mid, entries.end()));
    }

    /**
     * @brief Splits the node.
     *
     * @param entry The entry to be inserted.
     * @param distance The distance function.
     */
    void split(const TreeObjectPtr &entry, Metric distance) {
        
        // The new routing objects are now all those in the node plus the new routing object
        std::vector<TreeObjectPtr> allEntries = entries;
        allEntries.push_back(entry);

        // Create a new node
        maxNodeId = maxNodeId + 1;
        auto newNode = createNewNode(maxNodeId);

        // PROMOTE
        // Promote two entries to routing objects
        std::pair<TreeObjectPtr, TreeObjectPtr> promoted = promote(allEntries);
        TreeObjectPtr p1 = promoted.first;
        TreeObjectPtr p2 = promoted.second;

        // PARTITION
        // Divides entries of the overflown node into two disjoint sets
        std::pair<std::vector<TreeObjectPtr>, std::vector<TreeObjectPtr>> partEntries = partition(allEntries, p1, p2);
        std::vector<TreeObjectPtr> entries1 = partEntries.first;
        std::vector<TreeObjectPtr> entries2 = partEntries.second;

        // Store each partitioned entry in the corresponding node
        this->entries.clear();
        storeEntries(entries1, this); //  @TODO: Maybe this can be done faster, by just removing the elements that are in entries 2
        storeEntries(entries2, newNode);


        INSDEBUG_MSG("New Node" << newNode->getNodeId() << ", Promoted " << p1->getRepresentative() << " and " << p2->getRepresentative());

        INSDEBUG_INLINE("Partition Node" << this->getNodeId() << " (size " << this->entries.size() << ") [");
        for (const auto &entry : this->entries) {
            INSDEBUG_INLINE(entry->getRepresentative() << " ");
        }
        INSDEBUG_INLINE("] ");
        INSDEBUG_INLINE("Partition Node" << newNode->getNodeId() << " (size " << newNode->entries.size() << ") [");
        for (const auto &entry : newNode->entries) {
            INSDEBUG_INLINE(entry->getRepresentative() << " ");
        }
        INSDEBUG_INLINE("]\n");

        if (isRoot) {
            // The split occurred in the root node
            // In this case, create a new root node and the height of the tree increases by one
            maxNodeId = maxNodeId + 1;
            auto newRoot = createNewRootNode(maxNodeId);
            newRoot->setIsRoot(true);
            INSDEBUG_MSG("Splitting the root... NewRoot = " << newRoot->getNodeId());

            // Update coveringRadius, subtree and distanceToParent for each new routing object
            updateRoutingObject(p1, entries1, this->shared_from_this(), newRoot, distance);
            updateRoutingObject(p2, entries2, newNode, newRoot, distance);

            // Reset flag of the old root
            isRoot = false;

            INSDEBUG_MSG("Node" << p1->getSubtree()->getNodeId() << " linked to routing obj " << p1->getRepresentative() << " (id: " << newRoot->getNodeId() << "), covering radius " << p1->getCoveringRadius());
            
            INSDEBUG_MSG("Node" << p2->getSubtree()->getNodeId() << " linked to routing obj " << p2->getRepresentative() << " (id: " << newRoot->getNodeId() << "), covering radius " << p2->getCoveringRadius());
        } else {
            // The split occurred in an internal node or leaf node

            // In this case, start by replacing the parent routing object with the new routing object p1
            for (auto &entry : parentNode->entries) {
                if (entry == parentRoutingObj) {
                    // Remove the parent routing object from the parent node
                    parentNode->entries.erase(std::remove(parentNode->entries.begin(), parentNode->entries.end(), parentRoutingObj), parentNode->entries.end());

                    // Update the parent routing object with the new routing object p1
                    updateRoutingObject(p1, entries1, this->shared_from_this(), parentNode, distance);

                    // @TODO: Maybe this can be done faster too
                    INSDEBUG_MSG("Node" << p1->getSubtree()->getNodeId() << " linked to routing obj " << p1->getRepresentative() << " (id: " << parentNode->getNodeId() << "), covering radius " << p1->getCoveringRadius());
                    break;
                }
            }

            // Now decide what will happen with the new routing object p2

            // If the parent node has space for the new routing object, insert it
            if (parentNode->entries.size() < maxCapacity) {
                updateRoutingObject(p2, entries2, newNode, parentNode, distance);

                INSDEBUG_MSG("Node" << p2->getSubtree()->getNodeId() << " linked to routing obj " << p2->getRepresentative() << " (id: " << parentNode->getNodeId() << "), covering radius " << p1->getCoveringRadius());
            } else {
                // If the parent node is full, split it
                // Using the routing object p2 as the new tree entry

                // First, let p2 adopt the remaining entries
                // Update coveringRadius and subtree for the new routing object p2
                double maxDistance = 0.0;
                for (const auto &entry : entries2) {
                    double dist = distance(p2->getRepresentative(), entry->getRepresentative());
                    if (dist > maxDistance) {
                        maxDistance = dist;
                    }
                    // Update parentDistance for each entry
                    entry->setDistanceToParent(dist);
                }
                p2->setCoveringRadius(maxDistance);
                p2->setSubtree(newNode);
                newNode->setParentRoutingObj(p2);
                
                // Split the parent node to insert the new routing object p2
                parentNode->split(p2, distance);
            }
        }
    }

protected:
    size_t nodeId;
    size_t maxCapacity;
    bool isRoot;
    bool isLeaf;
    std::vector<TreeObjectPtr> entries;
    NodePtr parentNode;
    TreeObjectPtr parentRoutingObj;

    /**
     * @brief Stores entries in a node.
     *
     * @param newEntries The entries to be stored.
     * @param node The node in which to store the entries.
     */
    void storeEntries(const std::vector<TreeObjectPtr> &newEntries, NodePtr node) {
        for (const auto &entry : newEntries) {
            node->entries.push_back(entry);
        }
    }

    /**
     * @brief Stores entries in a node (non-shared_ptr version).
     *
     * @param newEntries The entries to be stored.
     * @param node The node in which to store the entries.
     */
    void storeEntries(const std::vector<TreeObjectPtr> &newEntries, Node<T>* node) {
        for (const auto &entry : newEntries) {
            node->entries.push_back(entry);
        }
    }
};

/**
 * @brief Class for a leaf node in the M-Tree.
 *
 * @tparam T The type of elements stored in the M-Tree.
 */
template <typename T>
class LeafNode : public Node<T>
{
public:
    typedef std::shared_ptr<TreeObject<T>> TreeObjectPtr;
    typedef std::shared_ptr<Node<T>> NodePtr;
    typedef std::function<double(const T &, const T &)> Metric;


    /**
     * @brief Constructs a LeafNode.
     *
     * @param nodeId The unique ID of the node.
     * @param maxCapacity The maximum number of elements the node can hold.
     * @param isRoot Whether the node is the root node.
     * @param parentNode The parent node.
     * @param parentRoutingObj The parent routing object.
     */
    LeafNode(size_t nodeId, size_t maxCapacity, bool isRoot, NodePtr parentNode, TreeObjectPtr parentRoutingObj);

    /**
     * @brief Inserts an element into the leaf node directly.
     * If the node is full, it will split the node.
     *
     * @param element The element to be inserted.
     * @param distance The distance function.
     */
    void insert(const T &element, Metric distance) override;

    /**
     * @brief Creates a new node.
     *
     * @param nodeId The unique ID of the new node.
     * @return A shared pointer to the new node.
     */
    NodePtr createNewNode(size_t nodeId) const override;

    /**
     * @brief Creates a new root node.
     *
     * @param nodeId The unique ID of the new root node.
     * @return A shared pointer to the new root node.
     */
    NodePtr createNewRootNode(size_t nodeId) const override;

    /**
     * @brief Updates the routing object.
     *
     * @param p The routing object to be updated.
     * @param entries The partitioned entries.
     * @param childNode The child node.
     * @param parentNode The parent node.
     * @param distance The distance function.
     */
    void updateRoutingObject(TreeObjectPtr p, std::vector<TreeObjectPtr> entries, NodePtr childNode, NodePtr parentNode, Metric distance) override;

    void search(const T &query, double dmin, NNList<T> &nnList, std::vector<std::pair<NodePtr, double>> &candidates, Metric distance) const override;

    /**
     * @brief Gets the string representation of the leaf node.
     *
     * @param os The output stream.
     */
    void getRepr(std::ostream &os) const override;
};

/**
 * @brief Class for an internal node in the M-Tree.
 *
 * @tparam T The type of elements stored in the M-Tree.
 */
template <typename T>
class InternalNode : public Node<T>
{
public:
    typedef std::shared_ptr<TreeObject<T>> TreeObjectPtr;
    typedef std::shared_ptr<Node<T>> NodePtr;
    typedef std::function<double(const T &, const T &)> Metric;

    /**
     * @brief Constructs an InternalNode.
     * 
     * An internal node is a node that contains routing objects, in contrast to a leaf node that contains non-routing objects (the objects that were inserted into the tree by the user).
     *
     * @param nodeId The unique ID of the node.
     * @param maxCapacity The maximum number of elements the node can hold.
     * @param isRoot Whether the node is the root node.
     * @param parentNode The parent node.
     * @param parentRoutingObj The parent routing object.
     */
    InternalNode(size_t nodeId, size_t maxCapacity, bool isRoot, NodePtr parentNode, TreeObjectPtr parentRoutingObj);

    /**
     * @brief Inserts an element into the internal node.
     * This insertion is done recursively, starting from the root node.
     * The criteria for inserting an element into a subtree is based on the distance to the routing object of the subtree.
     *      1. If the element fits into an existing entry, insert it into the subtree which minimizes the distance to the routing object.
     *      2. If the element does not fit into any existing entry, insert it into the subtree which minimizes the increase in the covering radius.
     *
     * @param element The element to be inserted.
     * @param distance The distance function.
     */
    void insert(const T &element, Metric distance) override;

    /**
     * @brief Creates a new node.
     *
     * @param nodeId The unique ID of the new node.
     * @return A shared pointer to the new node.
     */
    NodePtr createNewNode(size_t nodeId) const override;

    /**
     * @brief Creates a new root node.
     *
     * @param nodeId The unique ID of the new root node.
     * @return A shared pointer to the new root node.
     */
    NodePtr createNewRootNode(size_t nodeId) const override;

    /**
     * @brief Updates the routing object.
     *
     * @param p The routing object to be updated.
     * @param entries The partitioned entries.
     * @param childNode The child node.
     * @param parentNode The parent node.
     * @param distance The distance function.
     */
    void updateRoutingObject(TreeObjectPtr p, std::vector<TreeObjectPtr> entries, NodePtr childNode, NodePtr parentNode, Metric distance) override;

    void search(const T &query, double dmin, NNList<T> &nnList, std::vector<std::pair<NodePtr, double>> &candidates, Metric distance) const override;

    /**
     * @brief Gets the string representation of the internal node.
     *
     * @param os The output stream.
     */
    void getRepr(std::ostream &os) const override;
};

#include "MTreeInternalNode.hpp"
#include "MTreeLeafNode.hpp"

#endif // MTREENODES_HPP