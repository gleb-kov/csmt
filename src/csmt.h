#ifndef CSMT_CSMT_H
#define CSMT_CSMT_H

#include <cstdint>
#include <memory>
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
 * rewrite with no recursion
 * std::move ?
 * blob as aggregator ?
 * leaf_hash?
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

template <typename HashPolicy = DefaultHashPolicy, typename ValueType = std::string
          /*, typename Alloc = std::allocator<void>*/>
class Csmt {
public:
    /* Structure that holds key and value as element of merkle tree */
    struct Blob {
        const uint64_t key_;
        ValueType value_;

        Blob(uint64_t key, ValueType value)
            : key_(key)
            , value_(std::move(value)) {
        }
    };

private:
    struct Node {
        using ptr_t = std::unique_ptr<Node>;

        Blob blob_;
        ptr_t left_ = nullptr;
        ptr_t right_ = nullptr;

        explicit Node(Blob blob, ptr_t left, ptr_t right)
            : blob_(std::move(blob))
            , left_(std::move(left))
            , right_(std::move(right)) {
        }

        [[nodiscard]] bool is_leaf() const {
            return left_ == nullptr && right_ == nullptr;
        }

        [[nodiscard]] uint64_t get_key() const {
            return blob_.key_;
        }

        [[nodiscard]] ValueType get_value() const {
            return blob_.value_;
        }
    };

    using ptr_t = typename Node::ptr_t;

    // using NodeAlloc = typename std::allocator_traits<Alloc>
    //      ::template rebind_alloc<Node>;
    // using NodeAllocTraits = std::allocator_traits<NodeAlloc>;

    ptr_t root_ = nullptr;
    size_t size_ = 0;

private:
    uint64_t log2(uint64_t num) {
#ifdef __GNUC__
        return ((unsigned)(8 * sizeof(unsigned long long) - __builtin_clzll((num)) - 1));
#else
        static constexpr uint64_t table[64] = {
            0,  58, 1,  59, 47, 53, 2,  60, 39, 48, 27, 54, 33, 42, 3,  61,
            51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22, 4,  62,
            57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21, 56,
            45, 25, 31, 35, 16, 9,  12, 44, 24, 15, 8,  23, 7,  6,  5,  63};
        num |= num >> 1u;
        num |= num >> 2u;
        num |= num >> 4u;
        num |= num >> 8u;
        num |= num >> 16u;
        num |= num >> 32u;
        return table[(num * 0x03f6eaf2cd271461) >> 58u];
#endif
    }

    uint64_t distance(uint64_t lhs, uint64_t rhs) {
        return log2(lhs ^ rhs);
    }

private:
    static ptr_t make_node(const Blob &blob) {
        return std::make_unique<Node>(blob, nullptr, nullptr);
    }

    static ptr_t make_node(ptr_t &lhs, ptr_t &rhs) {
        uint64_t l_key = lhs->get_key();
        uint64_t r_key = rhs->get_key();
        uint64_t key = (l_key < r_key ? r_key : l_key);

        ValueType value = HashPolicy::merge_hash(lhs->get_value(), rhs->get_value());
        return std::make_unique<Node>(Blob(key, value), std::move(lhs), std::move(rhs));
    }

    static ptr_t make_node(ptr_t &root) {
        return make_node(root->left_, root->right_);
    }

private:
    ptr_t insert(ptr_t &root, const Blob &blob) {
        if (root->is_leaf()) {
            return insert_leaf(root, blob);
        }

        uint64_t l_key = root->left_->get_key();
        uint64_t r_key = root->right_->get_key();
        uint64_t l_dist = distance(blob.key_, l_key);
        uint64_t r_dist = distance(blob.key_, r_key);

        if (l_dist == r_dist) {
            ptr_t new_node = make_node(blob);
            uint64_t min_key = (l_key < r_key ? l_key : r_key);
            if (blob.key_ < min_key) {
                return make_node(new_node, root);
            } else {
                return make_node(root, new_node);
            }
        }

        if (l_dist < r_dist) {
            root->left_ = insert(root->left_, blob);
        } else {
            root->right_ = insert(root->right_, blob);
        }
        return make_node(root);
    }

    ptr_t insert_leaf(ptr_t &leaf, const Blob &blob) {
        uint64_t leaf_key = leaf->get_key();
        if (blob.key_ == leaf_key) {
            // update existing value
            leaf->blob_.value_ = blob.value_;
            return std::move(leaf);
        }
        ++size_;
        ptr_t new_node = make_node(blob);
        if (blob.key_ < leaf_key) {
            return make_node(new_node, leaf);
        } else {
            return make_node(leaf, new_node);
        }
    }

    ptr_t erase(ptr_t &root, uint64_t key) {
        if (root->is_leaf()) {
            if (root->get_key() == key) {
                --size_;
                return nullptr;
            } else {
                return std::move(root);
            }
        }
        if (root->left_ && root->left_->is_leaf() && root->left_->get_key() == key) {
            --size_;
            return std::move(root->right_);
        }
        if (root->right_ && root->right_->is_leaf() && root->right_->get_key() == key) {
            --size_;
            return std::move(root->left_);
        }

        uint64_t l_dist = distance(key, root->left_->get_key());
        uint64_t r_dist = distance(key, root->right_->get_key());

        if (l_dist == r_dist) {
            return std::move(root);
        }

        // in worst case the same pointer returned with move
        if (l_dist < r_dist) {
            root->left_ = erase(root->left_, key);
        } else {
            root->right_ = erase(root->right_, key);
        }
        return make_node(root);
    }

    /*bool contains(ptr_t root, uint64_t key) {
        if (root->is_leaf()) {
            return root->get_key() == key;
        }
        uint64_t left_key = root->left->get_key();
        uint64_t right_key = root->right->get_key();
        uint64_t l_dist = distance(key, left_key);
        uint64_t r_dist = distance(key, right_key);
        if (l_dist == r_dist) {
            return false; // TODO: decide branch
        }
        if (l_dist < r_dist) {
            return contains(root->left, key);
        } else {
            return contains(root->right, key);
        }
    }*/

public:
    Csmt() = default;

    void insert(uint64_t key, const ValueType &value) {
        if (root_) {
            root_ = insert(root_, {key, value});
        } else {
            ++size_;
            root_ = make_node({key, value});
        }
    }

    void erase(uint64_t key) {
        if (root_) {
            root_ = erase(root_, key);
        }
    }

    /*bool contains(uint64_t key) const {
        if (root_) {
            return contains(root_, key);
        } else {
            return false;
        }
    }*/

    std::vector<ValueType> membership_proof(uint64_t key) const {
        // FIXME
        UNUSED(key);
        return {};
    }

    size_t size() const {
        return size_;
    }

    ~Csmt() = default;
};

#endif // CSMT_CSMT_H
