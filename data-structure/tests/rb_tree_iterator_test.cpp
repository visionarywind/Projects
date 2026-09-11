#include <vector>

#include "rbtree/rb_tree.hpp"
#include "test_support.hpp"

void run_iterator_tests() {
    TEST_SECTION("iterators");
    rbtree::rb_tree<int> tree;
    CHECK(tree.begin() == tree.end());
    for (int value : {8, 3, 10, 1, 6, 14, 4, 7, 13}) {
        tree.insert(value);
    }

    const std::vector<int> expected{1, 3, 4, 6, 7, 8, 10, 13, 14};
    std::vector<int> actual;
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        actual.push_back(*it);
    }
    CHECK_EQ(actual, expected);

    auto it = tree.end();
    --it;
    CHECK_EQ(*it, 14);
    --it;
    CHECK_EQ(*it, 13);

    const auto& const_tree = tree;
    std::vector<int> const_values;
    for (auto const_it = const_tree.cbegin(); const_it != const_tree.cend(); ++const_it) {
        const_values.push_back(*const_it);
    }
    CHECK_EQ(const_values, expected);
    CHECK(tree.find(6) != tree.end());
    CHECK(tree.find(99) == tree.end());
}
