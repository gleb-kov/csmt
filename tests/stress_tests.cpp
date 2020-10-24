#include "contrib/gtest/gtest.h"
#include "src/csmt.h"
#include "utils.h"

#include <bitset>
#include <functional>
#include <random>
#include <unordered_set>

TEST(stress, spam_insert) {
    constexpr size_t OPERATIONS = 10000;

    std::random_device random_device;
    std::mt19937 generator(random_device());

    std::uniform_int_distribution<uint64_t> key_gen(0, std::numeric_limits<uint64_t>::max());
    std::function<std::string(size_t)> value_gen = [](size_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    Csmt<> tree;

    for (size_t iter = 0; iter < OPERATIONS; iter++) {
        uint64_t key = key_gen(generator);

        EXPECT_NO_THROW(tree.insert(key, value_gen(key)));
        ASSERT_TRUE(tree.contains(key));
    }
}

TEST(stress, spam_erase) {
    constexpr size_t OPERATIONS = 10000;

    std::random_device random_device;
    std::mt19937 generator(random_device());

    std::uniform_int_distribution<uint64_t> key_gen(0, std::numeric_limits<uint64_t>::max());
    std::function<std::string(size_t)> value_gen = [](size_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    Csmt<> tree;
    std::unordered_set<uint64_t> keys;

    for (size_t iter = 0; iter < OPERATIONS; iter++) {
        uint64_t key = key_gen(generator);
        while (keys.count(key)) {
            key = key_gen(generator);
        }
        keys.insert(key);
        tree.insert(key, value_gen(key));
    }

    for (uint64_t key : keys) {
        EXPECT_NO_THROW(tree.erase(key));
        ASSERT_FALSE(tree.contains(key));
    }
}

TEST(stress, spam_membership_proof) {
    constexpr size_t OPERATIONS = 10000;

    std::random_device random_device;
    std::mt19937 generator(random_device());

    std::uniform_int_distribution<uint64_t> key_gen(0, std::numeric_limits<uint64_t>::max());
    std::function<std::string(uint64_t)> value_gen = [](uint64_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    Csmt<> tree;
    std::unordered_set<uint64_t> keys;

    for (size_t iter = 0; iter < OPERATIONS; iter++) {
        uint64_t key = key_gen(generator);
        while (keys.count(key)) {
            key = key_gen(generator);
        }
        keys.insert(key);
        tree.insert(key, value_gen(key));
        EXPECT_NO_THROW(auto proof = tree.membership_proof(key));
    }

    for (uint64_t key : keys) {
        EXPECT_NO_THROW(auto proof = tree.membership_proof(key));
    }
}

TEST(stress, comeback) {
    constexpr size_t KEYS = 6000;

    std::function<std::string(uint64_t)> value_gen = [](uint64_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    Csmt<> tree;

    for (size_t key_index = 0; key_index < KEYS; ++key_index) {
        tree.insert(key_index, value_gen(key_index));
    }
    for (size_t key_index = KEYS / 2; key_index < KEYS; ++key_index) {
        ASSERT_TRUE(tree.contains(key_index));
    }
    for (size_t key_index = 0; key_index < KEYS / 2; ++key_index) {
        ASSERT_TRUE(tree.contains(key_index));
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
        tree.insert(key_index, value_gen(key_index));
    }
    for (size_t key_index = 0; key_index < KEYS; ++key_index) {
        ASSERT_TRUE(tree.contains(key_index));
    }
}

TEST(stress, pool) {
    constexpr size_t KEYS = 100;
    constexpr size_t OPERATIONS = 10000;

    std::random_device random_device;
    std::mt19937 generator(random_device());

    std::uniform_int_distribution<> op_gen(0, 2);
    std::uniform_int_distribution<uint64_t> key_gen(0, KEYS - 1);
    std::function<std::string(uint64_t)> value_gen = [](uint64_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    Csmt<> tree;
    std::bitset<KEYS> in_tree;
    size_t in_tree_size = 0;

    for (size_t op_index = 0; op_index < OPERATIONS; ++op_index) {
        int op = op_gen(generator);
        uint64_t key = key_gen(generator);

        if (op == 0) {
            // std::cerr << "insert " << key << std::endl;
            tree.insert(key, value_gen(key));
            if (!in_tree[key]) {
                in_tree[key] = true;
                ++in_tree_size;
            }
        } else if (op == 1) {
            // std::cerr << "erase " << key << std::endl;
            tree.erase(key);
            if (in_tree[key]) {
                in_tree[key] = false;
                --in_tree_size;
            }
        } else if (op == 2) {
            bool verdict = tree.contains(key);
            // std::cerr << "contains " << key << ' ' << verdict << std::endl;
            ASSERT_EQ(in_tree[key], verdict);
            ASSERT_EQ(in_tree_size, tree.size());
        }
    }
}
