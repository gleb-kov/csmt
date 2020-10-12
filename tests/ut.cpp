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

TEST(empty, blank_ops) {
    // useful for sanitizer
    Csmt<HashPolicySHA256> tree;

    ASSERT_FALSE(tree.contains(0));
    auto proof1 = tree.membership_proof(0);
    ASSERT_TRUE(proof1.empty());

    tree.erase(0);

    ASSERT_FALSE(tree.contains(0));
    auto proof2 = tree.membership_proof(0);
    ASSERT_TRUE(proof2.empty());
}

/*
 * TEST CASES:
 *
 * insert to empty, proof, erase, proof
 *
 * insert existing, proof, erase, proof
 *
 * update existing
 *
 * insert A to empty, proof A, erase B, proof A, proof B
 *
 * many inserts to empty, proof with reorder, erase some, proof, insert back, proof
 *
 * pool with keys and data, hold everything in map, random operations. ? how to verify proof?
 *
 *
 * unit test for minimal path part
 *
 * any corner cases?
 */


/* FIXME: add contains along with proof call */