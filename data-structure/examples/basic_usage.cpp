#include <iostream>

#include "rbtree/rb_tree.hpp"

int main() {
    rbtree::rb_tree<int> tree;
    for (int value : {10, 4, 18, 2, 7, 15, 21}) {
        tree.insert(value);
    }

    std::cout << "in-order:";
    for (int value : tree) {
        std::cout << ' ' << value;
    }
    std::cout << '\n';

    std::cout << "contains 7: " << std::boolalpha << tree.contains(7) << '\n';
    tree.erase(4);
    std::cout << "after erase 4, size: " << tree.size() << '\n';
    std::cout << "invariants: " << tree.verify_invariants() << '\n';
}
