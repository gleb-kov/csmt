#include "contrib/crypto/sha256.h"
#include "contrib/gtest/gtest.h"
#include "src/csmt.h"

#include <functional>

struct IdentityHashPolicy {
    static std::string leaf_hash(std::string leaf_value) {
        return leaf_value;
    }

    static std::string merge_hash(const std::string& lhs, const std::string& rhs) {
        return lhs + rhs;
    }
};

template <typename HP, typename VT>
bool look_for_key(const Csmt<HP, VT> &tree, uint64_t key, const std::vector<VT> &proof = {}) {
    bool empty = proof.empty();
    bool contains = tree.contains(key);
    if (empty == contains) {
        return false;
    }
    // return tree.membership_proof(key) == proof; FIXME
    return true;
}

TEST(empty, blank_erase) {
    Csmt tree;

    ASSERT_EQ(tree.size(), 0u);
    ASSERT_TRUE(look_for_key(tree, 0u));

    tree.erase(0);

    ASSERT_EQ(tree.size(), 0u);
    ASSERT_TRUE(look_for_key(tree, 0u));
}

TEST(basic, insert_erase) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(0, "hello");

    ASSERT_EQ(tree.size(), 1u);
    ASSERT_TRUE(look_for_key(tree, 0, {"hello"}));

    tree.erase(0);

    ASSERT_EQ(tree.size(), 0u);
    ASSERT_TRUE(look_for_key(tree, 0u));
}

TEST(basic, update) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(0, "hello");

    ASSERT_EQ(tree.size(), 1u);
    ASSERT_TRUE(look_for_key(tree, 0, {"hello"}));

    tree.insert(0, "world");

    ASSERT_EQ(tree.size(), 1u);
    ASSERT_TRUE(look_for_key(tree, 0, {"world"}));
}

TEST(basic, two_nodes) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(2, "hello");
    tree.insert(3, "world");

    ASSERT_TRUE(tree.contains(2));

    ASSERT_EQ(tree.size(), 2u);
    ASSERT_TRUE(look_for_key(tree, 2, {"hello", "helloworld"}));
    ASSERT_TRUE(look_for_key(tree, 3, {"world", "helloworld"}));

    tree.erase(6);
    ASSERT_EQ(tree.size(), 2u);
    ASSERT_TRUE(look_for_key(tree, 0));
    ASSERT_TRUE(look_for_key(tree, 3, {"world"}));
}

TEST(basic, not_intersects) {
    Csmt tree;

    tree.insert(2, "hello");
    ASSERT_TRUE(look_for_key(tree, 2, {"hello"}));

    tree.erase(3);

    ASSERT_TRUE(tree.size() == 1);
    ASSERT_TRUE(look_for_key(tree, 2, {"hello"}));
    ASSERT_TRUE(look_for_key(tree, 3, {}));
}

TEST(stress, comeback) {
    Csmt tree;
    constexpr size_t KEYS = 10;

    std::function<std::string(size_t)> value_gen = [](size_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    for (size_t key_index = 0; key_index < KEYS; ++key_index) {
        tree.insert(key_index, value_gen(key_index));
    }
    for (size_t key_index= KEYS / 2; key_index < KEYS; ++key_index) {
        ASSERT_TRUE(look_for_key(tree, key_index, {value_gen(key_index)}));
    }
    for (size_t key_index= 0; key_index < KEYS / 2; ++key_index) {
        ASSERT_TRUE(look_for_key(tree, key_index, {value_gen(key_index)}));
    }
    for (size_t key_index = 0; key_index < KEYS; key_index += 3) {
        tree.erase(key_index);
    }
    for (size_t key_index = 0; key_index < KEYS; ++key_index) {
        if (key_index % 3 == 0) {
            ASSERT_TRUE(look_for_key(tree, key_index, {}));
        } else {
            ASSERT_TRUE(look_for_key(tree, key_index, {value_gen(key_index)}));
        }
    }
    for (size_t key_index = 0; key_index < KEYS; key_index += 3) {
        tree.insert(key_index, value_gen(key_index));
    }
    for (size_t key_index= 0; key_index < KEYS; ++key_index) {
        ASSERT_TRUE(look_for_key(tree, key_index, {value_gen(key_index)}));
    }
}

/*
 * TEST CASES:
 * pool with keys and data, hold everything in map, random operations. ? how to verify proof?
 *
 * any corner cases?
 */
