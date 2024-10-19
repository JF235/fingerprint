#ifndef MTREEINTERNALNODE_HPP
#define MTREEINTERNALNODE_HPP

#include "MTreeNodes.hpp"

template <typename T>
InternalNode<T>::InternalNode(size_t maxCapacity, bool isRoot, NodePtr parentNode, TreeObjectPtr parentRoutingObj)
    : Node<T>(maxCapacity, isRoot, parentNode, parentRoutingObj)
{
    this->isLeaf = false;
}

template <typename T>
void InternalNode<T>::insert(const T &element, Metric distance)
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

template <typename T>
typename InternalNode<T>::NodePtr InternalNode<T>::createNewNode() const
{
    return std::make_shared<InternalNode<T>>(this->maxCapacity, false, nullptr, nullptr);
}

template <typename T>
typename InternalNode<T>::NodePtr InternalNode<T>::createNewRootNode() const
{
    return std::make_shared<InternalNode<T>>(this->maxCapacity, true, nullptr, nullptr);
}

template <typename T>
void InternalNode<T>::getRepr(std::ostream &os) const
{
    os << this->getParentNode()->getNodeId();
    os << "<" << this->getParentRoutingObj() << "> -> ";
    os << this->getNodeId() << " ";
    for (size_t i = 0; i < this->entries.size(); i++)
    {
        
        os << "(" << this->entries[i]->getRepresentative() << "<" << this->entries[i] << ">:" << this->entries[i]->getCoveringRadius() << ":" << this->entries[i]->getDistanceToParent() << ")";
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

template <typename T>
void InternalNode<T>::updateRoutingObject(TreeObjectPtr p, std::vector<TreeObjectPtr> &entries, NodePtr childNode, NodePtr parentNode, Metric distance)
{
    // This is called when a promoted routing object "p" is stored in the new Node "parentNode"
    // For this to be suceeded

    // 1. The new covering radius of p is max{d(p, r) + rad(r) | for r in entries}, remembering r is a route object and rad(r) is the covering radius of r
    // 2. Here we can also update the distance to the parent for all entries
    double maxDistance = 0.0;
    for (const auto &entry : entries)
    {
        // This overestimates the covering radius of p, but guarantees that it will cover all entries
        double dist = distance(p->getRepresentative(), entry->getRepresentative());
        if (dist + entry->getCoveringRadius() > maxDistance)
        {
            maxDistance = dist + entry->getCoveringRadius();
        }
        // Update the distance to the parent of the entry object
        entry->setDistanceToParent(dist);
    }
    p->setCoveringRadius(maxDistance);

    // 3. childNode is the subtree of p
    p->setSubtree(childNode);
    
    // 4. Update the childNode's parent Node/routingObj
    childNode->setParentNode(parentNode);
    childNode->setParentRoutingObj(p);

    // 5. Add p to the parent node
    parentNode->getEntries().push_back(p);
    // @TODO: Maybe check if its full?

    
}

template <typename T>
void InternalNode<T>::search(const T &query, double dmin, NNList<T> &nnList, std::vector<std::pair<NodePtr, double>> &candidates, Metric distance) const
{
    // For all entries in node
    for (const auto &entry : this->entries)
    {
        double dk = nnList.getMaxDistance();

        // Check if is root
        double dEntryParent;
        double dQueryParent;
        if (this->isRoot)
        {
            // If it is root, set d(entry, parent) = 0 to avoid pruning
            dEntryParent = 0.0;
            dQueryParent = 0.0;
        }
        else
        {
            // If it is check inequality |d(entry, parent) - d(query, parent)| <= dk
            // This uses the triangle inequality to prune the search
            // and avoid calculating the distance between the query and the entry
            // in case the entry is not a candidate
            dEntryParent = entry->getDistanceToParent();
            dQueryParent = this->parentRoutingObj->calculateDistanceToQuery(query, distance);
            //dQueryParent = distance(query, this->parentRoutingObj->getRepresentative()); // @TODO TROCAR ISSO POR UMA METODO DE TREE OBJECT QUE RECEBE A QUERY E VERIFICA SE A QUERY CACHEADA É IGUAL, CASO POSITIVO RETORNA O VALOR CACHEADO
        }

        // Check condition for pruning without calculating the distance
        if (std::fabs(dQueryParent - dEntryParent) <= dk + entry->getCoveringRadius())
        {
            // If the entry is a candidate, calculate the distance
            double dist = distance(query, entry->getRepresentative());
            // Compute the lower bound for entry
            double dminEntry = std::max(0.0, dist - entry->getCoveringRadius());
            // Check if the entry is a candidate using the lower bound
            if (dminEntry <= dk)
            {
                // Add to the candidate list
                candidates.emplace_back(entry->getSubtree(), dminEntry);

                // Compute the upper bound for entry
                double dmaxEntry = dist + entry->getCoveringRadius();

                // Check if there will be (with certainty) a element in the subtree of entry
                // This happens when the maximum distance in the nearest neighbors 
                // list is greater than the upper bound
                if (dmaxEntry < dk)
                {
                    // Update the NN list with the new upper bound 
                    // nothing is really inserted, just the maximum distance is updated
                    nnList.insert(dmaxEntry);
                    dk = nnList.getMaxDistance();

// For debugging purposes print erased candidates
#ifdef KNNDEBUG
                    bool print = false;
                    for (auto it = candidates.begin(); it != candidates.end(); ++it)
                    {
                        if (it->second > dk)
                        {
                            if (!print)
                            {
                                std::cout << "Erased candidates: ";
                                print = true;
                            }
                            std::cout << it->first->getNodeId() << " ";
                        }
                    }
                    if (print)
                        std::cout << std::endl;
#endif

                    // Prune the candidates for which the lower bound is greater than the new dk
                    candidates.erase(std::remove_if(candidates.begin(), candidates.end(), [dk](const std::pair<NodePtr, double> &candidate)
                                                    { return candidate.second > dk; }),
                                     candidates.end());
                }
            }
        }
    }
}

#endif // MTREEINTERNALNODE_HPP