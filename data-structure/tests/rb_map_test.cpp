#include <map>

#include "rbtree/rb_map.hpp"
#include "test_support.hpp"

void run_map_tests() {
    TEST_SECTION("map API and sentinel tree");
    rbtree::rb_map<int, int> tree;
    CHECK(tree.empty());
    CHECK(tree.verify_invariants());

    auto first = tree.try_emplace(4, 40);
    CHECK(first.second);
    CHECK_EQ(first.first->second, 40);
    auto duplicate = tree.try_emplace(4, 400);
    CHECK(!duplicate.second);
    CHECK_EQ(tree.find(4)->second, 40);

    tree[2] = 20;
    tree[7] = 70;
    tree[1];
    CHECK_EQ(tree.size(), 4U);
    CHECK(tree.verify_invariants());

    std::vector<int> keys;
    for (const auto& entry : tree) {
        keys.push_back(entry.first);
    }
    CHECK_EQ(keys, std::vector<int>({1, 2, 4, 7}));

    auto end = tree.end();
    --end;
    CHECK_EQ(end->first, 7);
    CHECK_EQ(end->second, 70);

    CHECK_EQ(tree.erase(4), 1U);
    CHECK_EQ(tree.erase(4), 0U);
    CHECK(tree.verify_invariants());
    tree.clear();
    CHECK(tree.empty());
    CHECK(tree.verify_invariants());
}
