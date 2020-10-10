#include "contrib/gtest/gtest.h"
#include "src/csmt.h"

TEST(empty, blank_ops) {
    // useful for sanitizer
    csmt tree;
    tree.erase(0);
    auto empty_proof = tree.membership_proof(0);
    ASSERT_TRUE(empty_proof.empty());
}

/*
 * TEST CASES:
 *
 * insert to empty, proof, erase, proof
 *
 * insert existing, proof, erase, proof
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
