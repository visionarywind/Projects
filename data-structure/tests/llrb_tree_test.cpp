#include <random>
#include <set>
#include <vector>

#include "rbtree/llrb_tree.hpp"
#include "test_support.hpp"

namespace {

std::vector<int> values(const rbtree::llrb_tree<int>& tree) {
    return std::vector<int>(tree.begin(), tree.end());
}

void check_matches(const rbtree::llrb_tree<int>& actual,
                   const std::set<int>& expected) {
    CHECK(actual.verify_invariants());
    CHECK_EQ(actual.size(), expected.size());
    CHECK_EQ(values(actual), std::vector<int>(expected.begin(), expected.end()));
}

}  // namespace

void run_llrb_tree_tests() {
    TEST_SECTION("left-leaning red-black tree");
    rbtree::llrb_tree<int> tree;
    std::set<int> expected;

    CHECK(tree.empty());
    CHECK(!tree.erase(1));
    for (int value : {30, 20, 10, 40, 50, 25, 35, 5, 15, 45}) {
        CHECK_EQ(tree.insert(value), expected.insert(value).second);
        check_matches(tree, expected);
    }
    CHECK(!tree.insert(30));
    CHECK(tree.contains(25));
    CHECK(!tree.contains(26));

    for (int value : {5, 50, 30, 20, 35, 10, 15, 25, 40, 45}) {
        CHECK_EQ(tree.erase(value), expected.erase(value) != 0U);
        check_matches(tree, expected);
    }

    std::mt19937 generator(20260911U);
    std::uniform_int_distribution<int> key_distribution(-100, 100);
    std::uniform_int_distribution<int> operation_distribution(0, 99);
    for (int operation = 0; operation < 20000; ++operation) {
        const int key = key_distribution(generator);
        const int choice = operation_distribution(generator);
        if (choice < 45) {
            CHECK_EQ(tree.insert(key), expected.insert(key).second);
        } else if (choice < 75) {
            CHECK_EQ(tree.erase(key), expected.erase(key) != 0U);
        } else if (choice < 95) {
            CHECK_EQ(tree.contains(key), expected.find(key) != expected.end());
        } else {
            tree.clear();
            expected.clear();
        }
        check_matches(tree, expected);
    }

    if (!expected.empty()) {
        auto iterator = tree.end();
        --iterator;
        CHECK_EQ(*iterator, *expected.rbegin());
        const auto begin = tree.begin();
        CHECK_EQ(*begin, *expected.begin());
    }
}
