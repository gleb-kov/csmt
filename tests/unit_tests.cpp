#include "contrib/crypto/sha256.h"
#include "contrib/gtest/gtest.h"
#include "src/csmt.h"
#include "utils.h"

#include <functional>

TEST(sha256, correct) {
    std::vector<std::pair<std::string, std::string>> codes{
            {"",       "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"},
            {"banana", "b493d48364afe44d11c0165cf470a4164d1e2609911ef998be868d46ade3de4e"},
            {"cat",    "77af778b51abd4a3c51c5ddd97204a9c3ae614ebccb75a606c3b6865aed6744e"},
            {"polina", "b34cd4a97cfc6649655a00bfa9cd3fb3acb5f73e59498ee0475b4547db516131"}
    };

    for (auto const &pair: codes) {
        ASSERT_EQ(SHA256::hash(pair.first), pair.second);
    }
}

TEST(log, correct) {
    // implementation from csmt private function
    std::function<uint64_t(uint64_t)> log_impl = [](uint64_t num) {
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
    };

    uint64_t key = 1;
    uint64_t next_key = 2;
    uint64_t power = 0;

    for (; key < (1ull << 15u); key++) {
        if (key == next_key) {
            next_key <<= 1u;
            power++;
        }
        ASSERT_EQ(log_impl(key), power);
    }
}

TEST(basic, blank_erase) {
    Csmt<> tree;

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
    ASSERT_TRUE(look_for_key(tree, 2, {"hello", "world", "helloworld"}));
    ASSERT_TRUE(look_for_key(tree, 3, {"hello", "world", "helloworld"}));

    tree.erase(6);
    ASSERT_EQ(tree.size(), 2u);
    ASSERT_TRUE(look_for_key(tree, 0));
    ASSERT_TRUE(look_for_key(tree, 3, {"hello", "world", "helloworld"}));
}

TEST(basic, two_nodes_erase) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(2, "hello");
    tree.insert(3, "world");

    ASSERT_TRUE(tree.contains(2));

    ASSERT_EQ(tree.size(), 2u);
    ASSERT_TRUE(look_for_key(tree, 2, {"hello", "world", "helloworld"}));
    ASSERT_TRUE(look_for_key(tree, 3, {"hello", "world", "helloworld"}));

    tree.erase(2);
    ASSERT_EQ(tree.size(), 1u);
    ASSERT_TRUE(look_for_key(tree, 0));
    ASSERT_TRUE(look_for_key(tree, 3, {"world"}));
}

TEST(basic, not_intersects) {
    Csmt<IdentityHashPolicy> tree;

    tree.insert(2, "hello");
    ASSERT_TRUE(look_for_key(tree, 2, {"hello"}));

    tree.erase(3);

    ASSERT_TRUE(tree.size() == 1);
    ASSERT_TRUE(look_for_key(tree, 2, {"hello"}));
    ASSERT_TRUE(look_for_key(tree, 3, {}));
}

TEST(basic, insert_trick) {
    std::function<std::string(uint64_t)> value_gen = [](uint64_t key_index) {
        return "VALUE" + std::to_string(key_index);
    };

    Csmt<> tree;
    tree.insert(12, value_gen(12));
    tree.insert(13, value_gen(13));
    tree.insert(12, value_gen(12));
    ASSERT_TRUE(tree.contains(13));
}

TEST(basic, binary_tree_proof) {
    std::function<std::string(uint64_t)> value_gen = [](uint64_t key_index) {
        return std::to_string(key_index);
    };

    Csmt<IdentityHashPolicy> tree;
    for (uint64_t key_index = 0; key_index < 8; ++key_index) {
        tree.insert(key_index, value_gen(key_index));
    }

    ASSERT_TRUE(look_for_key(tree, 0, {"0", "1", "01", "23", "0123", "4567", "01234567"}));
    ASSERT_TRUE(look_for_key(tree, 1, {"0", "1", "01", "23", "0123", "4567", "01234567"}));
    ASSERT_TRUE(look_for_key(tree, 5, {"4", "5", "45", "67", "0123", "4567", "01234567"}));
    ASSERT_TRUE(look_for_key(tree, 6, {"6", "7", "45", "67", "0123", "4567", "01234567"}));
}
