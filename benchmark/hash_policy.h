#ifndef CSMT_HASH_POLICY_H
#define CSMT_HASH_POLICY_H

#include "contrib/crypto/sha256.h"

struct HashPolicySHA256 {
    static std::string leaf_hash(std::string leaf_value) {
        return SHA256::hash(std::move(leaf_value));
    }

    static std::string merge_hash(const std::string &lhs, const std::string &rhs) {
        return SHA256::hash(lhs + rhs);
    }
};

struct HashPolicySHA256Tree {
    static std::string leaf_hash(const std::string &leaf_value) {
        return SHA256::hash("0" + leaf_value);
    }

    static std::string merge_hash(const std::string &lhs, const std::string &rhs) {
        return SHA256::hash("1" + lhs + "2" + rhs);
    }
};


#endif // CSMT_HASH_POLICY_H
