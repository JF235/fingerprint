#ifndef TREEOBJECT_HPP
#define TREEOBJECT_HPP

#include <memory>
#include <functional>

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
    TreeObject(T representative, double coveringRadius, std::shared_ptr<Node<T>> subtree, double distanceToParent)
        : representative(representative), coveringRadius(coveringRadius), subtree(nullptr), distanceToParent(distanceToParent) {}
    virtual ~TreeObject() = default;
    
    const T& getRepresentative() const { return representative; }
    double getCoveringRadius() const { return coveringRadius; }
    double getDistanceToParent() const { return distanceToParent; }
    void setDistanceToParent(double distanceToParent) { this->distanceToParent = distanceToParent; }
    void setCoveringRadius(double coveringRadius) { this->coveringRadius = coveringRadius; }
    std::shared_ptr<Node<T>> getSubtree() const { return subtree; }
    void setSubtree(std::shared_ptr<Node<T>> subtree) { this->subtree = subtree; }


protected:
    T representative;
    double coveringRadius;
    std::shared_ptr<Node<T>> subtree;
    double distanceToParent;
};

/**
 * @brief Classe para um objeto de roteamento na M-Tree.
 * 
 * @tparam T O tipo dos elementos armazenados na M-Tree.
 */
template <typename T>
class RoutingObject : public TreeObject<T> {
public:
    RoutingObject(T representative, double coveringRadius, std::shared_ptr<Node<T>> subtree, double distanceToParent)
        : TreeObject<T>(representative, coveringRadius, subtree, distanceToParent) {}
};

/**
 * @brief Classe para um objeto folha na M-Tree.
 * 
 * @tparam T O tipo dos elementos armazenados na M-Tree.
 */
template <typename T>
class LeafObject : public TreeObject<T> {
public:
    LeafObject(T representative, double distanceToParent)
        : TreeObject<T>(representative, 0.0, nullptr, distanceToParent) {}

};

#endif // TREEOBJECT_HPP