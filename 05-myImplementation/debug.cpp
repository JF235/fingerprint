#include "MTree.hpp"
#include "distances.hpp"
#include "FloatNumber.hpp"
#include "debug_msg.hpp"

int main(){
    // Create 4 FloatNumber objects
    FloatNumber a(1.0);
    FloatNumber b(2.0);
    FloatNumber c(3.0);
    FloatNumber d(4.0);

    // Create a tree which element is float and distance function is manhattanDistance
    MTree<FloatNumber> mtree(3, manhattanDistance<FloatNumber>);

    // Insert the 4 FloatNumber objects into the tree
    mtree.insert(a);
    mtree.insert(b);
    mtree.insert(c);

    std::cout << mtree << std::endl;

    mtree.insert(d);

    std::cout << mtree << std::endl;

    return 0;
}