#ifndef MTREEINTERNALNODE_HPP
#define MTREEINTERNALNODE_HPP

#include "MTreeNodes.hpp"

template <typename T>
InternalNode<T>::InternalNode(size_t nodeId, size_t maxCapacity, bool isRoot, typename Node<T>::NodePtr parentNode, typename Node<T>::TreeObjectPtr parentRoutingObj)
    : Node<T>(nodeId, maxCapacity, isRoot, parentNode, parentRoutingObj) 
    {
        this->isLeaf = false;
    }

// Insert element into the internal node
template <typename T>
void InternalNode<T>::insert(const T &element, std::function<double(const T &, const T &)> distance)
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

// createNewNode function for internal node
template <typename T>
typename Node<T>::NodePtr InternalNode<T>::createNewNode(size_t nodeId) const
{
    return std::make_shared<InternalNode<T>>(nodeId, this->maxCapacity, false, this->parentNode, this->parentRoutingObj);
}

// Create New Root Node
template <typename T>
typename Node<T>::NodePtr InternalNode<T>::createNewRootNode(size_t nodeId) const
{
    return std::make_shared<InternalNode<T>>(nodeId, this->maxCapacity, false, this->parentNode, this->parentRoutingObj);
}

// getRepr function for internal node
template <typename T>
void InternalNode<T>::getRepr(std::ostream &os) const
{
    for (size_t i = 0; i < this->entries.size(); i++)
    {
        os << "(" << this->entries[i]->getRepresentative() << ":" << this->entries[i]->getCoveringRadius() << ")";
        // Check if null
        if (this->entries[i]->getSubtree() == nullptr)
        {
            os << "*";
        }
        os << " ";
    }

    // Recurse down the tree
    for (size_t i = 0; i < this->entries.size(); i++)
    {
        if (this->entries[i]->getSubtree() != nullptr)
        {
            os << "\n";
            this->entries[i]->getSubtree()->getRepr(os);
        }
    }
}

// updateRoutingObject function for internal node
template <typename T>
void InternalNode<T>::updateRoutingObject(TreeObjectPtr p, std::vector<TreeObjectPtr> entries, std::shared_ptr<Node<T>> childNode, std::shared_ptr<Node<T>> parentNode, std::function<double(const T &, const T &)> distance)
{
    // This is called when a promoted routing object "p" is stored in the new Node "node"
    // For this to be suceeded
    // 1. The new covering radius of p is max{d(p, r) + rad(r) | for r in entries}, remembring r is a route object and rad(r) is the covering radius of r
    // 2. The subtree of p is "node"
    // 3. The distanceToParent of p is d(p, p1) where p1 is the entry in entries that maximizes d(p, p1)

    // 1.
    double maxDistance = 0.0;
    for (const auto &entry : entries)
    {
        // This overestimates the covering radius of p, but guarantees that it will cover all entries
        double dist = distance(p->getRepresentative(), entry->getRepresentative()) + entry->getCoveringRadius();
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

#endif // MTREEINTERNALNODE_HPP