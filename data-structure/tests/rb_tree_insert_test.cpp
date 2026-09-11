#include <vector>

#include "rbtree/rb_tree.hpp"
#include "test_support.hpp"

namespace {

void check_sequence(const std::vector<int>& values) {
    rbtree::rb_tree<int> tree;
    for (int value : values) {
        CHECK(tree.insert(value));
        CHECK(tree.verify_invariants());
    }
    std::vector<int> actual;
    for (int value : tree) {
        actual.push_back(value);
    }
    std::vector<int> expected = actual;
    for (std::size_t i = 1; i < expected.size(); ++i) {
        CHECK(expected[i - 1] < expected[i]);
    }
    CHECK_EQ(actual, expected);
}

}  // namespace

void run_insert_tests() {
    TEST_SECTION("insert rotations and recolouring");
    check_sequence({30, 20, 10});  // left-left
    check_sequence({10, 20, 30});  // right-right
    check_sequence({30, 10, 20});  // left-right
    check_sequence({10, 30, 20});  // right-left
    check_sequence({10, 5, 15, 1, 6, 12, 20, 0, 2, 7, 11, 13, 18, 25});
}
