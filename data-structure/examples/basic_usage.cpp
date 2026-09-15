#include <iostream>

#include "rbtree/rb_tree.hpp"
#include "rbtree/skip_list.hpp"

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

    rbtree::skip_list<int> list;
    for (int value : {10, 4, 18, 2, 7, 15, 21}) {
        list.insert(value);
    }
    std::cout << "skip-list in-order:";
    for (int value : list) {
        std::cout << ' ' << value;
    }
    std::cout << '\n';
    list.erase(4);
    std::cout << "skip-list after erase 4, size: " << list.size() << '\n';
    std::cout << "skip-list invariants: " << list.verify_invariants() << '\n';
}
