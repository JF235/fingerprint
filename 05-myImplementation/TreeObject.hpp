#ifndef TREEOBJECT_HPP
#define TREEOBJECT_HPP

#include <memory>
#include <functional>
#include "indexing/DistanceFunction.hpp"

template <typename T>
class Node; // Forward declaration

/**
 * @brief Classe abstrata para um objeto de árvore na M-Tree.
 * 
 * @tparam T O tipo dos elementos armazenados na M-Tree.
 */
template <typename T>
class TreeObject {
public:
    typedef std::shared_ptr<Node<T>> NodePtr;

    TreeObject(const T &representative, double coveringRadius, double distanceToParent)
        : representative(representative), coveringRadius(coveringRadius), subtree(nullptr), distanceToParent(distanceToParent) {}
    virtual ~TreeObject() = default;
    
    const T& getRepresentative() const { return representative; }
    
    double getDistanceToParent() const { return distanceToParent; }
    void setDistanceToParent(double distanceToParent) { this->distanceToParent = distanceToParent; }
    
    double getCoveringRadius() const { return coveringRadius; }
    void setCoveringRadius(double coveringRadius) { this->coveringRadius = coveringRadius; }
    
    NodePtr getSubtree() const { return subtree; }
    void setSubtree(NodePtr subtree) { this->subtree = subtree; }
    
    NodePtr getCurrentNode() const { return currentNode; }
    void setCurrentNode(NodePtr currentNode) { this->currentNode = currentNode; }

    double calculateDistanceToQuery(const T &query, DistanceFunction<T> &distance) {
        // If the query is the same as the cached query, return the cached distance
        if (query == this->query) {
            return distanceToQuery;
        } else {
            // Otherwise, calculate the distance and cache the new query and distance
            distanceToQuery = distance(query, representative);
            this->query = query;
            return distanceToQuery;
        }
    }


protected:
    const T& representative;           //> The representative of this object
    double coveringRadius;      //> The covering radius of this object
    NodePtr subtree;            //> The subtree of this object
    NodePtr currentNode;        //> The node that contains this object
    double distanceToParent;    //> Distance to the parent node

    // Cache for distance calculation
    double distanceToQuery;     //> Cached distance to query
    T query;                    //> Cache the query for distance calculation
};

/**
 * @brief Classe para um objeto de roteamento na M-Tree.
 * 
 * @tparam T O tipo dos elementos armazenados na M-Tree.
 */
template <typename T>
class RoutingObject : public TreeObject<T> {
public:
    typedef std::shared_ptr<Node<T>> NodePtr;

    RoutingObject(const T &representative, double coveringRadius, double distanceToParent)
        : TreeObject<T>(representative, coveringRadius, distanceToParent) {}
};

/**
 * @brief Classe para um objeto folha na M-Tree.
 * 
 * @tparam T O tipo dos elementos armazenados na M-Tree.
 */
template <typename T>
class LeafObject : public TreeObject<T> {
public:
    typedef std::shared_ptr<Node<T>> NodePtr;

    LeafObject(const T &representative, double distanceToParent)
        : TreeObject<T>(representative, 0.0, distanceToParent) {}

};

#endif // TREEOBJECT_HPP