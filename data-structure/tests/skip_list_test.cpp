#include <cstdint>
#include <functional>
#include <set>
#include <vector>

#include "rbtree/skip_list.hpp"
#include "test_support.hpp"

namespace {

template <typename List>
std::vector<int> values(const List& list) {
    std::vector<int> result;
    for (int value : list) {
        result.push_back(value);
    }
    return result;
}

}  // namespace

void run_skip_list_tests() {
    TEST_SECTION("skip list API and levels");
    rbtree::skip_list<int> list(rbtree::skip_list<int>::key_compare{}, 7U);
    CHECK(list.empty());
    CHECK_EQ(list.size(), 0U);
    CHECK(list.verify_invariants());

    CHECK(list.insert(4));
    CHECK(list.insert(1));
    CHECK(list.insert(7));
    CHECK(!list.insert(4));
    CHECK(list.contains(1));
    CHECK(!list.contains(5));
    CHECK_EQ(*list.find(7), 7);
    CHECK(list.find(5) == list.end());
    CHECK_EQ(values(list), std::vector<int>({1, 4, 7}));
    CHECK(list.verify_invariants());

    CHECK_EQ(list.erase(4), 1U);
    CHECK_EQ(list.erase(4), 0U);
    CHECK_EQ(values(list), std::vector<int>({1, 7}));
    CHECK(list.verify_invariants());

    const auto& const_list = list;
    std::vector<int> const_values;
    for (auto iterator = const_list.cbegin(); iterator != const_list.cend();
         ++iterator) {
        const_values.push_back(*iterator);
    }
    CHECK_EQ(const_values, std::vector<int>({1, 7}));

    list.clear();
    CHECK(list.empty());
    CHECK(list.verify_invariants());
    CHECK(list.insert(9));
    CHECK_EQ(values(list), std::vector<int>({9}));

    TEST_SECTION("skip list differential operations");
    rbtree::skip_list<int, std::less<int>, 12> actual(std::less<int>{},
                                                       12345U);
    std::set<int> expected;
    std::uint32_t state = 0x12345678U;
    for (int step = 0; step < 20000; ++step) {
        state = state * 1664525U + 1013904223U;
        const int key = static_cast<int>(state % 401U) - 200;
        const bool do_insert = (state & 1U) != 0U;
        if (do_insert) {
            CHECK_EQ(actual.insert(key), expected.insert(key).second);
        } else {
            CHECK_EQ(actual.erase(key), expected.erase(key));
        }
        CHECK_EQ(actual.size(), expected.size());
        CHECK(actual.verify_invariants());
        CHECK_EQ(values(actual), std::vector<int>(expected.begin(), expected.end()));
    }
}
