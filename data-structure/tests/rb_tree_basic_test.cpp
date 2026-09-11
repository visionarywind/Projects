#include <vector>

#include "rbtree/rb_tree.hpp"
#include "test_support.hpp"

void run_basic_tests() {
    TEST_SECTION("basic operations");
    rbtree::rb_tree<int> tree;
    CHECK(tree.empty());
    CHECK_EQ(tree.size(), 0U);
    CHECK(!tree.contains(1));
    CHECK(!tree.erase(1));
    CHECK(tree.verify_invariants());

    CHECK(tree.insert(5));
    CHECK(!tree.empty());
    CHECK_EQ(tree.size(), 1U);
    CHECK(tree.contains(5));
    CHECK(!tree.insert(5));
    CHECK_EQ(tree.size(), 1U);
    CHECK(tree.verify_invariants());

    for (int value : {3, 8, 1, 4, 7, 9}) {
        CHECK(tree.insert(value));
        CHECK(tree.verify_invariants());
    }
    CHECK(!tree.insert(3));
    CHECK_EQ(tree.size(), 7U);

    CHECK(tree.erase(5));
    CHECK(!tree.contains(5));
    CHECK(tree.verify_invariants());
    tree.clear();
    CHECK(tree.empty());
    CHECK(tree.verify_invariants());
    tree.clear();
    CHECK(tree.insert(42));
    CHECK(tree.verify_invariants());
}
