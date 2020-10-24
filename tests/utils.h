#ifndef CSMT_TEST_UTILS_H
#define CSMT_TEST_UTILS_H

#include "src/csmt.h"

struct IdentityHashPolicy {
    static std::string leaf_hash(std::string leaf_value) {
        return leaf_value;
    }

    static std::string merge_hash(const std::string &lhs, const std::string &rhs) {
        return lhs + rhs;
    }
};

template <typename HP, typename HT, typename VT>
bool look_for_key(const Csmt<HP, HT, VT> &tree, uint64_t key, const std::deque<HT> &proof = {}) {
    bool empty = proof.empty();
    bool contains = tree.contains(key);
    if (empty == contains) {
        return false;
    }
    return tree.membership_proof(key) == proof;
}

#endif // CSMT_TEST_UTILS_H
