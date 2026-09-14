#include <map>
#include <random>
#include <vector>

#include "rbtree/rb_map.hpp"
#include "test_support.hpp"

namespace {
std::vector<std::pair<int, int>> values(const rbtree::rb_map<int, int>& tree) {
    std::vector<std::pair<int, int>> result;
    for (const auto& entry : tree) result.emplace_back(entry.first, entry.second);
    return result;
}

std::vector<std::pair<int, int>> values(const std::map<int, int>& tree) {
    return std::vector<std::pair<int, int>>(tree.begin(), tree.end());
}
}  // namespace

void run_map_differential_tests() {
    TEST_SECTION("rb_map differential test against std::map");
    constexpr unsigned seed = 20260911U;
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> key_distribution(-200, 200);
    std::uniform_int_distribution<int> operation_distribution(0, 99);
    rbtree::rb_map<int, int> actual;
    std::map<int, int> expected;

    for (int operation = 0; operation < 20000; ++operation) {
        const int key = key_distribution(generator);
        const int value = operation * 3;
        const int choice = operation_distribution(generator);
        if (choice < 35) {
            const auto a = actual.try_emplace(key, value);
            const auto e = expected.emplace(key, value);
            CHECK_EQ(a.second, e.second);
        } else if (choice < 50) {
            actual[key] = value;
            expected[key] = value;
        } else if (choice < 75) {
            CHECK_EQ(actual.erase(key), expected.erase(key));
        } else if (choice < 95) {
            const auto a = actual.find(key);
            const auto e = expected.find(key);
            CHECK_EQ(a == actual.end(), e == expected.end());
            if (e != expected.end()) CHECK_EQ(a->second, e->second);
        } else {
            actual.clear();
            expected.clear();
        }
        CHECK(actual.verify_invariants());
        CHECK_EQ(actual.size(), expected.size());
        CHECK_EQ(values(actual), values(expected));
    }
}
