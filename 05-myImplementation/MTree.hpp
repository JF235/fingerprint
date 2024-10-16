#ifndef MTREE_HPP
#define MTREE_HPP

#include <vector>
#include <memory>
#include <limits>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include "MTreeNodes.hpp"
#include "TreeObject.hpp"
#include "debug_msg.hpp"

size_t maxNodeId = 0; ///< Counter to generate unique IDs for nodes.

/**
 * @brief Class representing an M-Tree.
 *
 * The M-Tree is a data structure used for organizing and searching data in metric spaces.
 *
 * @tparam T The type of elements stored in the M-Tree.
 */
template <typename T>
class MTree
{
public:
    typedef std::function<double(const T &, const T &)> metric; ///< Function type for computing the distance between two elements.

    /**
     * @brief Constructs an M-Tree with a specified maximum node capacity and distance function.
     * By Default, the root node is initializaed as a leaf node (contains non-routing objects).
     *
     * @param maxNodeCapacity The maximum number of elements a node can hold.
     * @param distance A function that computes the distance between two elements of type T.
     *               The function should be a metric, i.e.
     *                  1. d(x, y) >= 0 for all x, y
     *                  2. d(x, y) = 0 if and only if x = y
     *                  3. d(x, y) = d(y, x) for all x, y
     *                  4. d(x, z) <= d(x, y) + d(y, z) for all x, y, z
     */
    MTree(size_t maxNodeCapacity, metric distance)
        : maxNodeCapacity(maxNodeCapacity), distance(distance), nextNodeId(0)
    {
        root = std::make_shared<LeafNode<T>>(nextNodeId++, maxNodeCapacity, true, nullptr, nullptr);
    }

    /**
     * @brief Inserts an element into the M-Tree.
     *
     * @param element The element to be inserted.
     */
    void insert(const T &element)
    {
        DEBUG_MSG("Inserting element " << element << " into the node " << root->getNodeId());

        // Inserts the element recursively via the root node
        root->insert(element, distance);

        // Check if root has to be updated, in case of a split
        if (root->getIsRoot() == false)
        {
            // If the old root is not the root anymore, search for the new root
            // by going up the tree until the new root is found
            std::shared_ptr<Node<T>> node = root;
            while (node->getIsRoot() == false)
                node = node->getParentNode();
            
            // Found the new root
            root = node;
            DEBUG_MSG("New root is node " << root->getNodeId());
        }
    }

    /**
     * @brief Overloads the stream insertion operator to print the M-Tree.
     *
     * @param os The output stream.
     * @param mtree The M-Tree to be printed.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &os, const MTree<T> &mtree)
    {
        // Print the M-Tree recursively starting from the root node
        os << "M-Tree:\n\n";
        os << *mtree.root;
        return os;
    }

private:
    size_t maxNodeCapacity;        ///< The maximum number of elements a node can hold.
    metric distance;               ///< Function to compute the distance between two elements.
    std::shared_ptr<Node<T>> root; ///< Pointer to the root node of the M-Tree.
    size_t nextNodeId;             ///< Counter to generate unique IDs for nodes.
};

#endif // MTREE_HPP