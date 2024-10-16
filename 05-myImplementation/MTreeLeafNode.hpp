#ifndef MTREELEAFNODE_HPP
#define MTREELEAFNODE_HPP

#include "MTreeNodes.hpp"

template <typename T>
LeafNode<T>::LeafNode(size_t nodeId, size_t maxCapacity, bool isRoot, typename Node<T>::NodePtr parentNode, typename Node<T>::TreeObjectPtr parentRoutingObj)
    : Node<T>(nodeId, maxCapacity, isRoot, parentNode, parentRoutingObj) 
    {
        this->isLeaf = true;
    }

template <typename T>
void LeafNode<T>::insert(const T &element, std::function<double(const T &, const T &)> distance)
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

// Create New Node
template <typename T>
typename Node<T>::NodePtr LeafNode<T>::createNewNode(size_t nodeId) const
{
    return std::make_shared<LeafNode<T>>(nodeId, this->maxCapacity, false, this->parentNode, this->parentRoutingObj);
}

// Create New Root Node
template <typename T>
typename Node<T>::NodePtr LeafNode<T>::createNewRootNode(size_t nodeId) const
{
    return std::make_shared<InternalNode<T>>(nodeId, this->maxCapacity, false, this->parentNode, this->parentRoutingObj);
}

template <typename T>
void LeafNode<T>::getRepr(std::ostream &os) const
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

template <typename T>
void LeafNode<T>::updateRoutingObject(TreeObjectPtr p, std::vector<TreeObjectPtr> entries, std::shared_ptr<Node<T>> childNode, std::shared_ptr<Node<T>> parentNode, std::function<double(const T &, const T &)> distance)
{
    // This is called when a promoted routing object "p" is stored in the new Node "node"
    // For this to be suceeded
    // 1. The new covering radius of p is max{d(p, p1) | for p1 in entries}
    // 2. The subtree of p is "node"
    // 3. The distanceToParent of p is d(p, p1) where p1 is the entry in entries that maximizes d(p, p1)

    // 1.
    double maxDistance = 0.0;
    for (const auto &entry : entries)
    {
        double dist = distance(p->getRepresentative(), entry->getRepresentative());
        if (dist > maxDistance)
        {
            maxDistance = dist;
        }
    }
    p->setCoveringRadius(maxDistance);

    // 2.
    p->setSubtree(childNode);

    // 3.
    // If its parent is the root

    // Now the nodes must be update also
    parentNode->getEntries().push_back(p);
    // maybe check if its full

    childNode->setParentNode(parentNode);
    childNode->setParentRoutingObj(p);
}

#endif // MTREELEAFNODE_HPP