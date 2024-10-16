#ifndef MTREE_HPP
#define MTREE_HPP

#include <vector>
#include <memory>
#include <limits>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include "MTreeNodes.hpp" // Inclui a definição dos nós
#include "TreeObject.hpp" // Inclui a definição do RoutingObject
#include "debug_msg.hpp" // Inclui a definição das mensagens de debug

size_t maxNodeId = 0;

/**
 * @brief Classe para a M-Tree.
 * 
 * @tparam T O tipo dos elementos armazenados na M-Tree.
 */
template <typename T>
class MTree {
public:
    MTree(size_t maxNodeCapacity, std::function<double(const T&, const T&)> distance)
        : maxNodeCapacity(maxNodeCapacity), distance(distance), nextNodeId(0) {
        // Cria o primeiro nó como folha e raiz
        root = std::make_shared<LeafNode<T>>(nextNodeId++, maxNodeCapacity, true);
    }

    void insert(const T& element) {
        DEBUG_MSG("Inserting element " << element << " into the node " << root->getNodeId());
        maxNodeId = nextNodeId - 1;
        root->insert(element, distance);
        
        // Check if root has to be updated
        if (root->getIsRoot() == false)
        {
            // Goes up the tree to find the new root
            std::shared_ptr<Node<T>> node = root;
            while (node->getIsRoot() == false)
            {
                node = node->getParentNode();
            }
            root = node;

            DEBUG_MSG("New root is node " << root->getNodeId());
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const MTree<T> &mtree) {
        os << "M-Tree:\n\n";
        os << *mtree.root;
        return os;
    }

private:
    size_t maxNodeCapacity;
    std::function<double(const T&, const T&)> distance;
    std::shared_ptr<Node<T>> root;
    size_t nextNodeId; // Para gerar IDs únicos para os nós automaticamente
};

#endif // MTREE_HPP