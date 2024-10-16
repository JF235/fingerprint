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
    NodePtr getParentNode() const { return parentNode; }
    void setIsRoot(bool root) { isRoot = root; }
    virtual NodePtr createNewNode(size_t nodeId) const = 0;
    virtual NodePtr createNewRootNode(size_t nodeId) const = 0;
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
            DEBUG_MSG("Splitting in the root: create a level new root node " << newRoot->getNodeId());

            // For each new routing object (p1, p2), update coveringRadius, subtree and distanceToParent
            
            // The new covering radius of p1 is max{d(p, p1) | for p in entries1}
            double maxDistance1 = 0.0;
            for (const auto &entry : entries1)
            {
                double dist = distance(p1->getRepresentative(), entry->getRepresentative());
                if (dist > maxDistance1)
                {
                    maxDistance1 = dist;
                }
            }
            p1->setCoveringRadius(maxDistance1);
            p1->setSubtree(this->shared_from_this());

            // The same for p2
            double maxDistance2 = 0.0;
            for (const auto &entry : entries2)
            {
                double dist = distance(p2->getRepresentative(), entry->getRepresentative());
                if (dist > maxDistance2)
                {
                    maxDistance2 = dist;
                }
            }
            p2->setCoveringRadius(maxDistance2);
            p2->setSubtree(newNode);


            newRoot->setIsRoot(true);
            newRoot->entries.push_back(p1);
            newRoot->entries.push_back(p2);

            // Set newRoot as parent of this and newNode
            this->parentNode = newRoot;
            newNode->parentNode = newRoot;

            // Set p1 and p2 as parentRoutingObj of this and newNode
            this->parentRoutingObj = p1;
            newNode->parentRoutingObj = p2;

            // Reset flag of the old root
            isRoot = false;
        }
        else
        {
            // Replace parentRoutingObj with p1
            parentRoutingObj = p1;
            // Check if parent node is full
            if (parentNode->entries.size() < maxCapacity)
            {
                // Store p2 in parent
                parentNode->entries.push_back(p2);
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
    LeafNode(size_t nodeId, size_t maxCapacity, bool isRoot = false, typename Node<T>::NodePtr parentNode = nullptr, typename Node<T>::TreeObjectPtr parentRoutingObj = nullptr)
        : Node<T>(nodeId, maxCapacity, isRoot, parentNode, parentRoutingObj) {}

    void insert(const T &element, std::function<double(const T &, const T &)> distance) override
    {
        if (this->entries.size() < this->maxCapacity)
        {
            this->entries.push_back(std::make_shared<LeafObject<T>>(element, 0.0)); // distanceToParent is 0.0 for new elements
            DEBUG_MSG("Inserted element " << element << " into the leaf node " << this->getNodeId());
            DEBUG_MSG("Leaf node " << this->getNodeId() << " has now " << this->entries.size() << " elements");
        }
        else
        {
            DEBUG_MSG("Leaf node " << this->getNodeId() << " is full. Cannot insert element " << element);
            this->split(std::make_shared<LeafObject<T>>(element, 0.0), distance);
            // Check if root has to be updated

        }
    }

    bool isLeaf() const override
    {
        return true;
    }

    // Create New Node
    typename Node<T>::NodePtr createNewNode(size_t nodeId) const override
    {
        return std::make_shared<LeafNode<T>>(nodeId, this->maxCapacity, false, this->parentNode, this->parentRoutingObj);
    }


    // Create New Root Node
    typename Node<T>::NodePtr createNewRootNode(size_t nodeId) const override
    {
        return std::make_shared<InternalNode<T>>(nodeId, this->maxCapacity, false, this->parentNode, this->parentRoutingObj);
    }

    void getRepr(std::ostream &os) const override
    {
        for (size_t i = 0; i < this->entries.size(); i++)
        {
            os << "[" << this->entries[i]->getRepresentative() << "]";
            // Check if null
            if (this->entries[i]->getSubtree() == nullptr)
            {
                os << "*";
            }
            os << " ";
        }
    }
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
    InternalNode(size_t nodeId, size_t maxCapacity, bool isRoot = false, typename Node<T>::NodePtr parentNode = nullptr, typename Node<T>::TreeObjectPtr parentRoutingObj = nullptr)
        :  Node<T>(nodeId, maxCapacity, isRoot, parentNode, parentRoutingObj) {}

    void insert(const T &element, std::function<double(const T &, const T &)> distance) override
    {
        RoutingObject<T> *bestRoutingObject = nullptr;

        // The entry that the new object fits into
        RoutingObject<T> *bestRoutingObjectFit = nullptr;
        double minDistance = std::numeric_limits<double>::max();

        // The entry that will require the smallest increment in its covering radius
        RoutingObject<T> *bestRoutingObjectExpand = nullptr;
        double minRadiusIncrement = std::numeric_limits<double>::max();

        /* Look for entries that the new object fits into
        If there are no such entry, then look for an object with minimal distance from its covering radius's edge to the new object */
        for (auto &routingObject : this->entries)
        {
            double dist = distance(routingObject->getRepresentative(), element);
            if (dist <= routingObject->getCoveringRadius())
            {
                // Found an entry that the new object fits into
                if (dist < minDistance)
                {
                    // The closest entry
                    minDistance = dist;
                    bestRoutingObjectFit = dynamic_cast<RoutingObject<T> *>(routingObject.get());
                }
            }
            else if (!bestRoutingObjectFit)
            {
                // Still not found an entry that the new object fits into
                double radiusIncrement = dist - routingObject->getCoveringRadius();
                if (radiusIncrement < minRadiusIncrement)
                {
                    // This is the entry that will require the smallest increment in its covering radius
                    minRadiusIncrement = radiusIncrement;
                    bestRoutingObjectExpand = dynamic_cast<RoutingObject<T> *>(routingObject.get());
                }
            }
        }

        if (bestRoutingObjectFit)
        {
            // If it fits into an existing entry, just insert it into the corresponding subtree
            bestRoutingObject = bestRoutingObjectFit;
        }
        else
        {
            // If it doesn't fit into any existing entry, upgrade the new radii of the entry
            bestRoutingObjectExpand->setCoveringRadius(bestRoutingObjectExpand->getCoveringRadius() + minRadiusIncrement);

            bestRoutingObject = bestRoutingObjectExpand;
        }

        // Continue inserting in the next level
        bestRoutingObject->getSubtree()->insert(element, distance);
    }

    bool isLeaf() const override
    {
        return false;
    }

    // Create New Node
    typename Node<T>::NodePtr createNewNode(size_t nodeId) const override
    {
        return std::make_shared<InternalNode<T>>(nodeId, this->maxCapacity, false, this->parentNode, this->parentRoutingObj);
    }

    void getRepr(std::ostream &os) const override
    {
        os << " with " << this->entries.size() << " elements";
    }
};

#endif // MTREENODES_HPP