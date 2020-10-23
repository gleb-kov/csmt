#include "contrib/gtest/gtest.h"
#include "src/csmt.h"
#include "tests/common/test_utils.h"

#include <bitset>
#include <random>
#include <limits>
#include <unordered_set>

using namespace test;

TEST(stress, spam_insert) {
    Csmt<> tree;
    constexpr size_t OPERATIONS = 100000;

    std::uniform_int_distribution<uint64_t> key_gen(0, std::numeric_limits<uint64_t>::max());

    for (size_t iter = 0; iter < OPERATIONS; iter++) {
        uint64_t key = key_gen(rnd());

        EXPECT_NO_THROW(tree.insert(key, default_value_gen(key)));
        ASSERT_TRUE(tree.contains(key));
    }
}

TEST(stress, spam_erase) {
    Csmt<> tree;
    constexpr size_t OPERATIONS = 100000;

    std::uniform_int_distribution<uint64_t> key_gen(0, std::numeric_limits<uint64_t>::max());
    std::unordered_set<uint64_t> keys;

    for (size_t iter = 0; iter < OPERATIONS; iter++) {
        uint64_t key = key_gen(rnd());
        while (keys.count(key)) {
            key = key_gen(rnd());
        }
        keys.insert(key);
        tree.insert(key, default_value_gen(key));
    }

    for (uint64_t key: keys) {
        EXPECT_NO_THROW(tree.erase(key));
        ASSERT_FALSE(tree.contains(key));
    }
}

TEST(stress, spam_membership_proof) {
    Csmt<> tree;
    constexpr size_t OPERATIONS = 100000;

    std::uniform_int_distribution<uint64_t> key_gen(0, std::numeric_limits<uint64_t>::max());
    std::unordered_set<uint64_t> keys;

    for (size_t iter = 0; iter < OPERATIONS; iter++) {
        uint64_t key = key_gen(rnd());
        while (keys.count(key)) {
            key = key_gen(rnd());
        }
        keys.insert(key);
        tree.insert(key, default_value_gen(key));
        EXPECT_NO_THROW(tree.membership_proof(key));
    }

    for (uint64_t key: keys) {
        EXPECT_NO_THROW(tree.membership_proof(key));
    }
}

TEST(stress, comeback) {
    Csmt<> tree;
    constexpr size_t KEYS = 6000;

    for (size_t key_index = 0; key_index < KEYS; ++key_index) {
        tree.insert(key_index, default_value_gen(key_index));
    }
    for (size_t key_index = KEYS / 2; key_index < KEYS + KEYS / 2; ++key_index) {
        ASSERT_TRUE(tree.contains(key_index % KEYS));
    }
    for (size_t key_index = 0; key_index < KEYS; key_index += 3) {
        tree.erase(key_index);
    }
    for (size_t key_index = 0; key_index < KEYS; ++key_index) {
        if (key_index % 3 == 0) {
            ASSERT_TRUE(look_for_key(tree, key_index, {}));
        } else {
            ASSERT_TRUE(tree.contains(key_index));
        }
    }
    for (size_t key_index = 0; key_index < KEYS; key_index += 3) {
        tree.insert(key_index, default_value_gen(key_index));
    }
    for (size_t key_index = 0; key_index < KEYS; ++key_index) {
        ASSERT_TRUE(tree.contains(key_index));
    }
}

TEST(stress, pool) {
    Csmt<> tree;
    constexpr size_t KEYS = 100;
    constexpr size_t OPERATIONS = 10000;

    std::uniform_int_distribution<> op_gen(0, 2);
    std::uniform_int_distribution<uint64_t> key_gen(0, KEYS - 1);

    std::bitset<KEYS> in_tree;
    size_t in_tree_size = 0;

    for (size_t op_index = 0; op_index < OPERATIONS; ++op_index) {
        int op = op_gen(rnd());
        uint64_t key = key_gen(rnd());

        if (op == 0) {
            tree.insert(key, default_value_gen(key));
            if (!in_tree[key]) {
                in_tree[key] = true;
                ++in_tree_size;
            }
        } else if (op == 1) {
            tree.erase(key);
            if (in_tree[key]) {
                in_tree[key] = false;
                --in_tree_size;
            }
        } else if (op == 2) {
            bool verdict = tree.contains(key);
            ASSERT_EQ(in_tree[key], verdict);
            ASSERT_EQ(in_tree_size, tree.size());
        }
    }
}
