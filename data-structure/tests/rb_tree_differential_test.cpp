#include <algorithm>
#include <random>
#include <set>
#include <vector>

#include "rbtree/rb_tree.hpp"
#include "test_support.hpp"

namespace {

std::vector<int> values(const rbtree::rb_tree<int>& tree) {
    std::vector<int> result;
    for (int value : tree) {
        result.push_back(value);
    }
    return result;
}

std::vector<int> values(const std::set<int>& tree) {
    return std::vector<int>(tree.begin(), tree.end());
}

}  // namespace

void run_differential_tests() {
    TEST_SECTION("differential test against std::set");
    constexpr unsigned seed = 20260911U;
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> key_distribution(-100, 100);
    std::uniform_int_distribution<int> operation_distribution(0, 99);

    rbtree::rb_tree<int> actual;
    std::set<int> expected;
    for (int operation = 0; operation < 10000; ++operation) {
        const int key = key_distribution(generator);
        const int choice = operation_distribution(generator);
        if (choice < 45) {
            CHECK_EQ(actual.insert(key), expected.insert(key).second);
        } else if (choice < 75) {
            CHECK_EQ(actual.erase(key), expected.erase(key) != 0U);
        } else if (choice < 95) {
            CHECK_EQ(actual.contains(key), expected.find(key) != expected.end());
        } else {
            actual.clear();
            expected.clear();
        }

        CHECK(actual.verify_invariants());
        CHECK_EQ(actual.size(), expected.size());
        CHECK_EQ(values(actual), values(expected));
    }
}
