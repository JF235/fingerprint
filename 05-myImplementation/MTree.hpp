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
#include "includes/dbgmsg.hpp"
#include "indexing/NNList.hpp"
#include "indexing/DistanceFunction.hpp"

// Global variable for counting the number of nodes
extern size_t totalNodes;

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
    typedef std::shared_ptr<Node<T>> NodePtr;


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
    MTree(size_t maxNodeCapacity, DistanceFunction<T> &distance)
        : maxNodeCapacity(maxNodeCapacity), distance(distance), treeSize(0), height(1)
    {
        totalNodes = 0;
        root = std::make_shared<LeafNode<T>>(maxNodeCapacity, true, nullptr, nullptr);
    }

    /**
     * @brief Inserts an element into the M-Tree.
     *
     * @param element The element to be inserted.
     */
    void insert(const T &element)
    {
        INSDEBUG_MSG("Inserting element " << element << " into the node " << root->getNodeId());

        // Inserts the element recursively via the root node
        root->insert(element, distance);

        // Check if root has to be updated, in case of a split
        if (root->getIsRoot() == false)
        {
            // If the old root is not the root anymore, search for the new root
            // by going up the tree until the new root is found
            NodePtr node = root;
            while (node->getIsRoot() == false)
                node = node->getParentNode();
            
            root = node;
            INSDEBUG_MSG("New root is node " << root->getNodeId());

            // Update the height of the tree, because a split in the root node increases the height
            height++;
        }

        treeSize++;
    }

    /**
     * @brief Searches for the nearest neighbors of a query element in the M-Tree.
     *
     * @param query The query element.
     * @param k The number of nearest neighbors to search for.
     * @return A list of the k nearest neighbors.
     */
    NNList<T> knn(const T &query, size_t k) {
        // Benchmarking
        nodesAccessed = 0;

        /* Create a list to store the k nearest neighbors, initialized with infinite distance.
        The list is sorted in ascending order of distance to the query. */
        NNList<T> nnList(k, std::numeric_limits<double>::infinity());
        
        /* Create list of candidates that consists in a node and the lower 
        bound on the distance between the query and any object in the 
        subtree defined by the node.
            dmin = max{d(query, routingObj) - coveringRadius, 0}
        If dmin = 0, the query is in the subtree defined by the node. */
        std::vector<std::pair<NodePtr, double>> candidates;
        
        // The lower bound doesnt matter for the root node, so initialize it with 0
        candidates.emplace_back(root, 0.0);

        KNNDEBUG_MSG("KNN: " << nnList);
        #ifdef KNNDEBUG
        printCandidates(candidates);
        #endif

        // Search for the k nearest neighbors
        while (!candidates.empty())
        {
            // Select entry which the lower bound (dmin) is the smallest
            auto minCandidate = std::min_element(candidates.begin(), candidates.end(), [](const auto &a, const auto &b) {
                return a.second < b.second;
            });
            
            // Then remove the pair from the candidates list
            NodePtr node = minCandidate->first;
            // @ TODO: Why discard the lower bound? Can I be reused?
            //double dmin = minCandidate->second;
            candidates.erase(minCandidate);

            KNNDEBUG_MSG("Searching in Node" << node->getNodeId() << " (dmin = " << dmin << ", dk = " << nnList.getMaxDistance() << ")");

            nodesAccessed++;
            node->search(query, nnList, candidates, distance);

            KNNDEBUG_MSG("KNN: " << nnList);
            #ifdef KNNDEBUG
            printCandidates(candidates);
            #endif
        }
        
        return nnList;
    }

    /**
     * @brief Gets the number of elements in the M-Tree.
     *
     * @return The number of elements in the M-Tree.
     */
    size_t size() const
    {
        return treeSize;
    }

    /**
     * @brief Gets the height of the M-Tree.
     *
     * @return The height of the M-Tree.
     */
    size_t getHeight() const
    {
        return height;
    }

    /**
     * @brief Gets the number of nodes accessed during the last search.
     *
     * @return The number of nodes accessed during the last search.
     */
    size_t getNodesAccessed() const
    {
        return nodesAccessed;
    }

    /**
     * @brief Gets the total number of nodes in the M-Tree.
     *
     * @return The total number of nodes in the M-Tree.
     */
    size_t getTotalNodes() const
    {
        return totalNodes;
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
        os << "M-Tree:\n";
        os << *mtree.root;
        return os;
    }

private:

    void printCandidates(std::vector<std::pair<NodePtr, double>> &candidates) const
    {
        std::cout << "Candidates: ";
        for (const auto &candidate : candidates)
        {
            std::cout << "(" << candidate.first->getNodeId() << ", " << candidate.second << ") ";
        }
        std::cout << std::endl;
    }

    
    size_t maxNodeCapacity;         ///< The maximum number of elements a node can hold.
    DistanceFunction<T> &distance;   ///< Function that computes the distance between two elements of type T.
    size_t treeSize;                ///< Number of elements in the M-Tree.
    size_t height;                  ///< Height of the M-Tree.
    NodePtr root;                   ///< Pointer to the root node of the M-Tree.
    
    // Parameters for benchmarking
    size_t nodesAccessed;           ///< Number of nodes accessed during search.
};

#endif // MTREE_HPP