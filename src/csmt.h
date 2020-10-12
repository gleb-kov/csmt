#ifndef CSMT_CSMT_H
#define CSMT_CSMT_H

#include <cstdint>
#include <string>
#include <vector>

#define UNUSED(x) (void)(x)

/* WARNING: use it just for samples, not in real code */
struct DefaultHashPolicy {
    static std::string leaf_hash(const std::string &leaf_value) {
        return std::to_string(std::hash<std::string>{}(leaf_value));
    }

    static std::string merge_hash(const std::string &lhs, const std::string &rhs) {
        return std::to_string(std::hash<std::string>{}(lhs + rhs));
    }

    template <typename T>
    static T leaf_hash(const T &leaf_value) {
        return std::hash<T>{}(leaf_value);
    }

    template <typename T>
    static T merge_hash(const T &lhs, const T &rhs) {
        return std::hash<T>{}(lhs + rhs);
    }
};

/*
 * TODO:
 * custom allocator
 * fix raw pointers
 * set deleted pointers to nullptr
 * rewrite with no recursion
 * deallocate root?
 * std::move ?
 * blob as aggregator ?
 */

/*
 * HashPolicy requirements:
 * hash leaf
 * hash node as parent
 * just hash ?
 */

/*
 * Compact sparse merkle tree.
 *
 * Basic operations:
 *  insert(key, value)
 *  erase(key)
 *  contains(key)
 *  membership_proof(key)
 *
 * Requirements:
 *  HashPolicy -- type with static methods leaf_hash and merge_hash.
 *      leaf_hash to hash all origin elements in CSMT.
 *      merge_hash to hash two subnodes in CSMT.
 *
 *  Type returned by HashPolicy equals to origin type.
 *
 *  Key type -- uint64_t.
 */
template <typename HashPolicy = DefaultHashPolicy, typename ValueType = std::string>
class Csmt {
public:
    /* Structure that holds key and value as element of merkle tree */
    struct Blob {
        const uint64_t key_;
        ValueType value_;

        Blob(int64_t key, ValueType value)
            : key_(key)
            , value_(std::move(value)) {
        }
    };

private:
    struct Node {
        Blob blob_;
        Node *left = nullptr;
        Node *right = nullptr;

        explicit Node(Blob blob)
            : blob_(std::move(blob)) {
        }

        [[nodiscard]] bool is_leaf() const {
            return left == nullptr && right == nullptr;
        }

        [[nodiscard]] uint64_t get_key() const {
            return blob_.key_;
        }

        [[nodiscard]] ValueType get_value() const {
            return blob_.value_;
        }

        [[nodiscard]] bool check_for_leaf(uint64_t key) const {
            return (left->is_leaf() && left->get_key() == key) ||
                   (right->is_leaf() && right->get_key() == key);
        }
    };

    using ptr_t = Node *;

    ptr_t root_ = nullptr;

private:
    uint64_t log2(uint64_t num) {
        // TODO: add fast log2 and benchmark it
        int64_t res = 0;
        while (num) {
            num >>= 1u;
            ++res;
        }
        return res;
    }

    uint64_t distance(uint64_t lhs, uint64_t rhs) {
        return log2(lhs ^ rhs);
    }

private:
    static ptr_t alloc_node(const Blob &blob) {
        return new Node(blob);
    }

    static void dealloc_node(ptr_t node) {
        delete node;
    }

    static ptr_t make_node(const Blob &blob) {
        // TODO: rewrite me
        ptr_t node = alloc_node(blob);
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }

    static ptr_t make_node(ptr_t lhs, ptr_t rhs) {
        // TODO: rewrite me
        uint64_t lkey = lhs->get_key();
        uint64_t rkey = rhs->get_key();
        uint64_t key = (lkey < rkey ? rkey : lkey);

        ValueType value = HashPolicy::merge_hash(lhs->get_value(), rhs->get_value());
        ptr_t node = make_node(Blob(key, value));
        node->left = lhs;
        node->right = rhs;
        return node;
    }

private:
    ptr_t insert(ptr_t root, const Blob &blob) {
        if (root->is_leaf()) {
            return insert_leaf(root, blob);
        }
        uint64_t ldist = distance(blob.key_, root->left->get_key());
        uint64_t rdist = distance(blob.key_, root->right->get_key());
        if (ldist == rdist) {
            // FIXME
        }
        if (ldist < rdist) {
            root->left = insert(root->left, blob);
        } else {
            root->right = insert(root->right, blob);
        }
        ptr_t new_node = make_node(root->left, root->right);
        return new_node;
    }

    ptr_t insert_leaf(ptr_t leaf, const Blob &blob) {
        uint64_t leaf_key = leaf->get_key();
        if (blob.key_ == leaf_key) {
            // update existing value
            leaf->blob = blob;
            return leaf;
        }

        ptr_t new_node = make_node(blob);
        if (blob.key_ < leaf_key) {
            return make_node(new_node, leaf);
        } else {
            return make_node(leaf, new_node);
        }
    }

    ptr_t erase(ptr_t root, uint64_t key) {
        if (root->check_for_leaf(key)) {
            if (root->left->get_key() == key) {
                dealloc_node(root->left);
                return root->right;
            } else {
                dealloc_node(root->right);
                return root->left;
            }
        } else {
            uint64_t ldist = distance(key, root->left->get_key());
            uint64_t rdist = distance(key, root->right->get_key());
            if (ldist == rdist) {
                return root;
            }
            if (ldist < rdist) {
                root->left = erase(root->left, key);
            } else {
                root->right = erase(root->right, key);
            }
            ptr_t new_node = make_node(root->left, root->right);
            return new_node;
        }
    }

public:
    Csmt() = default;

    void insert(const Blob &b) {
        if (root_) {
            root_ = insert(root_, b);
        } else {
            root_ = make_node(b);
        }
    }

    void erase(uint64_t key) {
        if (root_) {
            root_ = erase(root_, key);
        }
    }

    bool contains(uint64_t key) {
        // FIXME
        UNUSED(key);
        return false;
    }

    std::vector<Blob> membership_proof(uint64_t key) {
        // FIXME
        UNUSED(key);
        return {};
    }
};

#endif // CSMT_CSMT_H
