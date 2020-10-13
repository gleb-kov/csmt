#include <utility>

#include "contrib/crypto/sha256.h"
#include "contrib/gtest/gtest.h"
#include "src/csmt.h"

struct HashPolicySHA256 {
    static std::string leaf_hash(std::string leaf_value) {
        return SHA256::hash(std::move(leaf_value));
    }

    static std::string merge_hash(const std::string& lhs, const std::string& rhs) {
        return SHA256::hash(lhs + rhs);
    }
};

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
    bool contains = /*tree.contains(key)*/ !empty; // FIXME
    return (empty != contains) && tree.membership_proof(key) == proof;
}

TEST(empty, blank_erase) {
    // useful for sanitizer
    Csmt<HashPolicySHA256> tree;

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
    // ASSERT_TRUE(look_for_key(tree, 0, {"hello"}));

    tree.erase(0);

    ASSERT_EQ(tree.size(), 0u);
    ASSERT_TRUE(look_for_key(tree, 0u));
}

TEST(basic, update) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(0, "hello");

    ASSERT_EQ(tree.size(), 1u);
    // ASSERT_TRUE(look_for_key(tree, 0, {"hello"}));

    tree.insert(0, "world");

    ASSERT_EQ(tree.size(), 1u);
    // ASSERT_TRUE(look_for_key(tree, 0, {"world"}));
}

TEST(basic, two_nodes) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(0, "hello");
    tree.insert(5, "world");

    ASSERT_EQ(tree.size(), 2u);
    // ASSERT_TRUE(look_for_key(tree, 0, {"hello", "helloworld"}));
    // ASSERT_TRUE(look_for_key(tree, 1, {"world", "helloworld"}));

    tree.erase(6);
    ASSERT_EQ(tree.size(), 2u);
    // ASSERT_TRUE(look_for_key(tree, 0));
    // ASSERT_TRUE(look_for_key(tree, 1, {"world"}));
}

/*
 * TEST CASES:
 *
 * insert A to empty, proof A, erase B, proof A, proof B
 *
 * many inserts to empty, proof with reorder, erase some, proof, insert back, proof
 *
 * pool with keys and data, hold everything in map, random operations. ? how to verify proof?
 *
 * any corner cases?
 */
