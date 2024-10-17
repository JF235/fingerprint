#ifndef MTREELEAFNODE_HPP
#define MTREELEAFNODE_HPP

#include "MTreeNodes.hpp"

template <typename T>
LeafNode<T>::LeafNode(size_t nodeId, size_t maxCapacity, bool isRoot, NodePtr parentNode, TreeObjectPtr parentRoutingObj)
    : Node<T>(nodeId, maxCapacity, isRoot, parentNode, parentRoutingObj) 
    {
        this->isLeaf = true;
    }

template <typename T>
void LeafNode<T>::insert(const T &element, Metric distance)
{
    if (this->entries.size() < this->maxCapacity)
    {
        // Check if its root
        if (this->isRoot)
        {
            // If it is root, set d(element, parent) = inf
            this->entries.emplace_back(std::make_shared<LeafObject<T>>(element, std::numeric_limits<float>::infinity()));

            // @TODO: (Remove) Get pointer value
            INSDEBUG_MSG("Inserted [element: " << element << "<" << this->entries.back() << ">" << ", distance: inf] " <<  "into root node [id: " << this->getNodeId() << ", size: " << this->entries.size() << "]");
            return;
        }
        else
        {
            // Get the distance to the parent
            double dist = distance(element, this->parentRoutingObj->getRepresentative());

            this->entries.emplace_back(std::make_shared<LeafObject<T>>(element, dist)); // distanceToParent is inf for new elements
            INSDEBUG_MSG("Inserted [element: " << element << "<" << this->entries.back() << ">" << ", dist2parent: " << dist << "] into node [id: " << this->getNodeId() << ", size: " << this->entries.size() << ", parent: " << this->parentRoutingObj->getRepresentative() << "]");
        }

    }
    else
    {
        //INSDEBUG_MSG("Leaf node " << this->getNodeId() << " is full. Cannot insert element " << element);
        INSDEBUG_MSG("Node " << this->getNodeId() << " Full and Splitting");
        this->split(std::make_shared<LeafObject<T>>(element, std::numeric_limits<float>::infinity()), distance);
    }
}

// Create New Node
template <typename T>
typename LeafNode<T>::NodePtr LeafNode<T>::createNewNode(size_t nodeId) const
{
    return std::make_shared<LeafNode<T>>(nodeId, this->maxCapacity, false, this->parentNode, this->parentRoutingObj);
}

// Create New Root Node
template <typename T>
typename LeafNode<T>::NodePtr LeafNode<T>::createNewRootNode(size_t nodeId) const
{
    return std::make_shared<InternalNode<T>>(nodeId, this->maxCapacity, false, this->parentNode, this->parentRoutingObj);
}

template <typename T>
void LeafNode<T>::getRepr(std::ostream &os) const
{
    os << this->getNodeId() << " ";
    os << this->getParentNode()->getNodeId();
    for (size_t i = 0; i < this->entries.size(); i++)
    {
        os << "[" << this->entries[i]->getRepresentative() << "<" << this->entries[i] << ">]";
        // Check if null
        if (this->entries[i]->getSubtree() == nullptr)
        {
            os << "*";
        }
        os << " ";
    }
}

template <typename T>
void LeafNode<T>::updateRoutingObject(TreeObjectPtr p, std::vector<TreeObjectPtr> &entries, NodePtr childNode, NodePtr parentNode, Metric distance)
{
    // This is called when a promoted routing object "p" is stored in the new Node "parentNode"
    // For this to be suceeded...

    // 1. The new covering radius of p is max{d(p, p1) | for p1 in entries}
    // This is true for the LeafNode case
    double maxDistance = 0.0;
    for (const auto &entry : entries)
    {
        double dist = distance(p->getRepresentative(), entry->getRepresentative());
        if (dist > maxDistance)
        {
            maxDistance = dist;
        }
        // The entry object is child of the routing object p
        // So update the disntanceToParent of the entry object
        entry->setDistanceToParent(dist);
    }
    p->setCoveringRadius(maxDistance);

    // 2. The subtree of p is "node"
    p->setSubtree(childNode);

    // 3. Add p to the parent node
    parentNode->getEntries().push_back(p);

    // 4. Update the childNode's parent Node/routingObj
    childNode->setParentNode(parentNode);
    childNode->setParentRoutingObj(p);
}

template <typename T>
void LeafNode<T>::search(const T &query, double dmin, NNList<T> &nnList, std::vector<std::pair<NodePtr, double>> &candidates, Metric distance) const
{
    // For all entries in node
    for (const auto &entry : this->entries)
    {
        // Let dk be the maxDistance in NNList
        double dk = nnList.getMaxDistance();

        // Check if is root
        double dEntryParent;
        double dQueryParent;
        if (this->isRoot){
            // If it is root, set d(entry, parent) = 0 to avoid pruning
            dEntryParent = 0.0;
            dQueryParent = 0.0;
        } else {
            // If it is check inequality |d(entry, parent) - d(query, parent)| <= dk
            dEntryParent = entry->getDistanceToParent();
            dQueryParent = distance(query, this->parentRoutingObj->getRepresentative());
        }

        // @TODO: If I store the distance once is computed, I can avoid this computation
        
        
        // It can prune without computing the distance
        if (std::fabs(dEntryParent - dQueryParent) <= dk)
        {
            // This condition comes from the triangle inequality

            // If true, compute d(entry, query)
            double dEntryQuery = distance(entry->getRepresentative(), query);

            // If this distance is less than or equal to dk
            if (dEntryQuery <= dk)
            {
                // Insert entry in NNList
                nnList.insert(entry->getRepresentative(), dEntryQuery);

                // Get new dk
                dk = nnList.getMaxDistance();

                // Prune all candidates which lower bound is greater than dk
                candidates.erase(std::remove_if(candidates.begin(), candidates.end(), [dk](const std::pair<NodePtr, double> &candidate) {
                    return candidate.second > dk;
                }), candidates.end());
            }
        }
    }


}

#endif // MTREELEAFNODE_HPP