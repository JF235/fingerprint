#ifndef MTREEINTERNALNODE_HPP
#define MTREEINTERNALNODE_HPP

#include "MTreeNodes.hpp"

template <typename T>
InternalNode<T>::InternalNode(size_t nodeId, size_t maxCapacity, bool isRoot, typename Node<T>::NodePtr parentNode, typename Node<T>::TreeObjectPtr parentRoutingObj)
    : Node<T>(nodeId, maxCapacity, isRoot, parentNode, parentRoutingObj) {}

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

// isLeaf function for internal node
template <typename T>
bool InternalNode<T>::isLeaf() const
{
    return false;
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
    os << " with " << this->entries.size() << " elements";
}

// updateRoutingObject function for internal node
template <typename T>
void InternalNode<T>::updateRoutingObject(TreeObjectPtr p, std::vector<TreeObjectPtr> entries, std::shared_ptr<Node<T>> childNode, std::shared_ptr<Node<T>> parentNode, std::function<double(const T &, const T &)> distance)
{
    // Do nothing
    int a = 0;
    a++;
}

#endif // MTREEINTERNALNODE_HPP