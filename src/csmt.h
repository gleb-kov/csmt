#ifndef CSMT_CSMT_H
#define CSMT_CSMT_H

// #include "contrib/crypto/sha256.h" TODO: pass functor into cmst as template

#include <cstdint>
#include <string>
#include <vector>

#define UNUSED(x) (void)(x)

struct blob {
    const int64_t key_;
    const std::string value_;

    blob(int64_t key, std::string value)
        : key_(key), value_(std::move(value)) {}
};

class csmt {
private:
    struct node {
        blob b;
        node *left = nullptr;
        node *right = nullptr;
    };

    node *root = nullptr;

public:
    csmt() = default;

    void insert(const blob &b) { UNUSED(b); }

    void erase(int64_t key) { UNUSED(key); }

    std::vector<blob> membership_proof(int64_t key) {
        UNUSED(key);
        return {};
    }
};

#endif // CSMT_CSMT_H
