#include <algorithm>
#include <vector>

#include "rbtree/rb_tree.hpp"
#include "test_support.hpp"

void run_erase_tests() {
    TEST_SECTION("erase cases");
    rbtree::rb_tree<int> tree;
    for (int value : {20, 10, 30, 5, 15, 25, 35, 3, 7, 13, 17, 23, 27, 33, 37}) {
        CHECK(tree.insert(value));
    }
    CHECK(tree.verify_invariants());

    for (int value : {3, 37, 5, 35, 10, 30, 20, 15, 25, 7, 13, 17, 23, 27, 33}) {
        CHECK(tree.erase(value));
        CHECK(!tree.contains(value));
        CHECK(tree.verify_invariants());
    }
    CHECK(tree.empty());
    CHECK(!tree.erase(100));

    for (int value = 0; value < 100; ++value) {
        CHECK(tree.insert(value));
    }
    for (int value = 99; value >= 0; --value) {
        CHECK(tree.erase(value));
        CHECK(tree.verify_invariants());
    }
    CHECK(tree.empty());
}
